#include "phyxunitsystem.h"

PhyxUnitSystem::PhyxUnitSystem(QObject *parent) :
    QObject(parent)
{
}

PhyxUnitSystem::~PhyxUnitSystem()
{
    // cleanup maps
    QMapIterator<QString, PhyxUnit*> i(baseUnitsMap);
    while (i.hasNext())
    {
        i.next();
        delete i.value();
    }
    QMapIterator<QString, PhyxUnit*> i2(derivedUnitsMap);
    while (i2.hasNext())
    {
        i2.next();
        delete i2.value();
    }
}

void PhyxUnitSystem::addBaseUnit(QString symbol, PhyxUnit::UnitFlags flags, QString unitGroup, QString preferedPrefix)
{
    if (baseUnitsMap.contains(symbol))
        delete baseUnitsMap.take(symbol);

   PhyxUnit *unit = new PhyxUnit();
   unit->setSymbol(symbol);
   unit->powerAppend(preferedPrefix + symbol,1);
   unit->setFlags(flags);
   unit->setUnitGroup(unitGroup);
   unit->setPreferedPrefix(preferedPrefix);
   baseUnitsMap.insert(symbol, unit);

    if (derivedUnitsMap.contains(symbol))
    {
        delete derivedUnitsMap.take(symbol);
        recalculate();
    }

    emit unitAdded(symbol);
}

/*void PhyxUnitSystem::addDerivedUnit(QString symbol, PhyxVariable *variable, double offset, PhyxUnit::UnitFlags flags)
{
    if (baseUnitsMap.contains(symbol))
        delete baseUnitsMap.take(symbol);

    PhyxUnit *unit;

    if (derivedUnitsMap.contains(symbol))
        unit = derivedUnitsMap.value(symbol);
    else
        unit = new PhyxUnit();

   variable->simplifyUnit();
   unit->setSymbol(symbol);
   unit->setPowers(variable->unit()->powers());
   unit->setScaleFactor(variable->value());
   unit->setOffset(offset);
   unit->setFlags(flags);
   derivedUnitsMap.insert(symbol, unit);

   recalculate();
}*/

void PhyxUnitSystem::addDerivedUnit(PhyxUnit *unit)
{
    if (baseUnitsMap.contains(unit->symbol()))
        delete baseUnitsMap.take(unit->symbol());

    if (derivedUnitsMap.contains(unit->symbol()))
        delete derivedUnitsMap.take(unit->symbol());

   derivedUnitsMap.insert(unit->symbol(), unit);
   recalculate();

   emit unitAdded(unit->symbol());
}

bool PhyxUnitSystem::removeUnit(QString symbol)
{
    if (baseUnitsMap.contains(symbol))
        delete baseUnitsMap.take(symbol);

    if (derivedUnitsMap.contains(symbol))
        delete derivedUnitsMap.take(symbol);

    recalculate();

    emit unitRemoved(symbol);

    return true;
}

void PhyxUnitSystem::addPrefix(QString symbol, double value, QString unitGroup, bool inputOnly)
{
    PhyxPrefix prefix;
    prefix.value = value;
    prefix.unitGroup = unitGroup;
    prefix.symbol = symbol;
    prefix.inputOnly = inputOnly;

    prefixMap.insert(symbol, prefix);
    emit prefixAdded(symbol);
}

bool PhyxUnitSystem::removePrefix(QString symbol)
{
    prefixMap.remove(symbol);
    emit prefixRemoved(symbol);
    return true;
}

void PhyxUnitSystem::addUnitGroup(QString name)
{
    if (!unitGroupsList.contains(name))
        unitGroupsList.append(name);
    emit unitGroupAdded(name);
}

bool PhyxUnitSystem::removeUnitGroup(QString name)
{
    unitGroupsList.removeOne(name);
    emit unitGroupRemoved(name);
    return true;
}

void PhyxUnitSystem::recalculateUnits()
{
}

void PhyxUnitSystem::recalculateVariables()
{
}

void PhyxUnitSystem::recalculate()
{
    recalculateUnits();
    recalculateVariables();
}

PhyxUnit * PhyxUnitSystem::copyUnit(QString symbol)
{
    if (baseUnitsMap.contains(symbol))
    {
        PhyxUnit *unit = new PhyxUnit();
        PhyxUnit::copyUnit(baseUnitsMap.value(symbol), unit);
        return unit;
    }
    else if (derivedUnitsMap.contains(symbol))
    {
        PhyxUnit *unit = new PhyxUnit();
        PhyxUnit::copyUnit(derivedUnitsMap.value(symbol), unit);
        return unit;
    }
    else
        return new PhyxUnit();
}

PhyxUnit *PhyxUnitSystem::unit(QString symbol)
{
    if (baseUnitsMap.contains(symbol))
        return baseUnitsMap.value(symbol);
    else if (derivedUnitsMap.contains(symbol))
        return derivedUnitsMap.value(symbol);
    else
        return new PhyxUnit();
}

PhyxUnitSystem::PhyxPrefix PhyxUnitSystem::prefix(QString symbol, QString unitGroup)
{
    PhyxPrefix prefix;
    prefix.value = 1;
    prefix.unitGroup = "";

    QList<PhyxPrefix> prefixList = prefixMap.values(symbol);
    for (int i = 0; i < prefixList.size(); i++)
    {
        if (prefixList.at(i).unitGroup == unitGroup)
            return prefixList.at(i);
    }
    return prefix;
}

QList<PhyxUnitSystem::PhyxPrefix> PhyxUnitSystem::prefixes(QString unitGroup) const
{
    QList<PhyxPrefix> prefixes;

    QMapIterator<QString, PhyxPrefix> mapIterator(prefixMap);
    while (mapIterator.hasNext())
    {
        mapIterator.next();
        if (mapIterator.value().unitGroup == unitGroup)
            prefixes.append(mapIterator.value());
    }

    //append the empty 1 prefix
    PhyxPrefix onePrefix;
    onePrefix.unitGroup = unitGroup;
    onePrefix.value = 1;
    onePrefix.symbol = "";
    onePrefix.inputOnly = false;
    prefixes.append(onePrefix);

    qSort(prefixes.begin(), prefixes.end());    // sort the prefixes
    return prefixes;
}

bool PhyxUnitSystem::verifyUnit(PhyxUnit *unit)
{
    if (unit->isBaseUnit())
    {
        QMapIterator<QString, PhyxUnit*> i(baseUnitsMap);
        while (i.hasNext())
        {
            i.next();
            if (i.value()->isSame(unit))
            {
                unit->setSymbol(i.value()->symbol());
                unit->setName(i.value()->name());
                unit->setFlags(i.value()->flags());
                return true;
            }
        }
    }
    else
    {
        QList<PhyxUnit*> matchList;
        QMapIterator<QString, PhyxUnit*> i(derivedUnitsMap);
        while (i.hasNext())
        {
            i.next();
            if (i.value()->isSame(unit))
                matchList.append(i.value());
        }

        if (!matchList.isEmpty())
        {
            PhyxUnit* match = NULL;
            if (matchList.size() > 1)   //handle multiple matches, currently the first match with unitGroup is used
            {
                for (int pos = 0; pos < matchList.size(); pos++)
                {
                    if (!matchList.at(pos)->unitGroup().isEmpty())
                    {
                        match = matchList[pos];
                        break;
                    }
                }
                if (match == NULL)
                    match = matchList.first();
            }
            else
                match = matchList.first();

            unit->setSymbol(match->symbol());
            unit->setName(match->name());
            unit->setFlags(match->flags());
            return true;
        }
    }

    return false;
}
