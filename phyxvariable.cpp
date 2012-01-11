#include "phyxvariable.h"

PhyxVariable::PhyxVariable(QObject *parent) :
    QObject(parent)
{
    m_value = 1;
    setUnit(new PhyxCompoundUnit());
}

PhyxVariable::~PhyxVariable()
{
    delete m_unit;
}

bool PhyxVariable::convertUnit(PhyxCompoundUnit *unit)
{
    if (!this->unit()->convertTo(unit))
    {
        m_error = PhyxVariable::UnitsNotConvertibleError;
        return false;
    }
    else
        return true;
}

QString PhyxVariable::errorString()
{
    switch (m_error)
    {
    case UnitsNotConvertibleError:      return tr("Units are not convertible");
    case UnitNotDimensionlessError:     return tr("Unit is not dimensionless");
    case ValueNotPositiveError:         return tr("value is not positive");
    case ValueNotIntegerError:          return tr("Value is not an integer");
    default:                            return tr("No error");
    }
}

bool PhyxVariable::mathAdd(PhyxVariable *variable)
{
    if (m_unit->add(variable->unit()))
    {
        m_value += variable->value();
        return true;
    }
    else
    {
        this->m_error = UnitsNotConvertibleError;
        return false;
    }
}

bool PhyxVariable::mathSub(PhyxVariable *variable)
{
    if (m_unit->sub(variable->unit()))
    {
        m_value -= variable->value();
        return true;
    }
    else
    {
        this->m_error = UnitsNotConvertibleError;
        return false;
    }
}

bool PhyxVariable::mathMul(PhyxVariable *variable)
{
    this->m_unit->multiply(variable->unit());
    this->m_value *= variable->value();

    return true;
}

bool PhyxVariable::mathDiv(PhyxVariable *variable)
{
    this->m_unit->divide(variable->unit());
    this->m_value /= variable->value();

    return true;
}

void PhyxVariable::mathNeg()
{
    m_value = -m_value;
}

bool PhyxVariable::mathRaise(PhyxVariable *variable)
{
    if (!variable->unit()->isDimensionlessUnit())
    {
        m_error = PhyxVariable::UnitNotDimensionlessError;
        return false;
    }
    //variable->simplifyUnit();

    this->unit()->raise(variable->value());
    this->m_value = pow(this->m_value, variable->value());

    return true;
}

bool PhyxVariable::mathRoot(PhyxVariable *variable)
{
    if (!variable->unit()->isDimensionlessUnit())
    {
        m_error = PhyxVariable::UnitNotDimensionlessError;
        return false;
    }

    this->unit()->root(variable->value());
    this->m_value = pow(this->m_value, 1.0/variable->value());

    return true;
}

void PhyxVariable::mathSqrt()
{
    this->unit()->root(2);
    this->m_value = sqrt(this->m_value);
}

bool PhyxVariable::mathSin()
{
    if (!this->unit()->isDimensionlessUnit())
    {
        m_error = PhyxVariable::UnitNotDimensionlessError;
        return false;
    }
    this->unit()->simplify();

    m_value = sin(m_value);
    return true;
}

bool PhyxVariable::mathArcsin()
{
    if (!this->unit()->isDimensionlessUnit())
    {
        m_error = PhyxVariable::UnitNotDimensionlessError;
        return false;
    }
    this->unit()->simplify();

    m_value = asin(m_value);
    return true;
}

bool PhyxVariable::mathCos()
{
    if (!this->unit()->isDimensionlessUnit())
    {
        m_error = PhyxVariable::UnitNotDimensionlessError;
        return false;
    }
    this->unit()->simplify();

    m_value = cos(m_value);
    return true;
}

bool PhyxVariable::mathArccos()
{
    if (!this->unit()->isDimensionlessUnit())
    {
        m_error = PhyxVariable::UnitNotDimensionlessError;
        return false;
    }
    this->unit()->simplify();

    m_value = acos(m_value);
    return true;
}

bool PhyxVariable::mathTan()
{
    if (!this->unit()->isDimensionlessUnit())
    {
        m_error = PhyxVariable::UnitNotDimensionlessError;
        return false;
    }
    this->unit()->simplify();

    m_value = tan(m_value);
    return true;
}

bool PhyxVariable::mathArctan()
{
    if (!this->unit()->isDimensionlessUnit())
    {
        m_error = PhyxVariable::UnitNotDimensionlessError;
        return false;
    }
    this->unit()->simplify();

    m_value = atan(m_value);
    return true;
}

bool PhyxVariable::mathSinh()
{
    if (!this->unit()->isDimensionlessUnit())
    {
        m_error = PhyxVariable::UnitNotDimensionlessError;
        return false;
    }
    this->unit()->simplify();

    m_value = sinh(m_value);
    return true;
}

bool PhyxVariable::mathArcsinh()
{
    if (!this->unit()->isDimensionlessUnit())
    {
        m_error = PhyxVariable::UnitNotDimensionlessError;
        return false;
    }
    this->unit()->simplify();

    m_value = asinh(m_value);
    return true;
}

bool PhyxVariable::mathCosh()
{
    if (!this->unit()->isDimensionlessUnit())
    {
        m_error = PhyxVariable::UnitNotDimensionlessError;
        return false;
    }
    this->unit()->simplify();

    m_value = cosh(m_value);
    return true;
}

bool PhyxVariable::mathArccosh()
{
    if (!this->unit()->isDimensionlessUnit())
    {
        m_error = PhyxVariable::UnitNotDimensionlessError;
        return false;
    }
    this->unit()->simplify();

    m_value = acosh(m_value);
    return true;
}

bool PhyxVariable::mathTanh()
{
    if (!this->unit()->isDimensionlessUnit())
    {
        m_error = PhyxVariable::UnitNotDimensionlessError;
        return false;
    }
    this->unit()->simplify();

    m_value = tanh(m_value);
    return true;
}

bool PhyxVariable::mathArctanh()
{
    if (!this->unit()->isDimensionlessUnit())
    {
        m_error = PhyxVariable::UnitNotDimensionlessError;
        return false;
    }
    this->unit()->simplify();

    m_value = atanh(m_value);
    return true;
}

bool PhyxVariable::mathExp()
{
    if (!this->unit()->isDimensionlessUnit())
    {
        m_error = PhyxVariable::UnitNotDimensionlessError;
        return false;
    }
    this->unit()->simplify();

    m_value = exp(m_value);
    return true;
}

bool PhyxVariable::mathLn()
{
    if (!this->unit()->isDimensionlessUnit())
    {
        m_error = PhyxVariable::UnitNotDimensionlessError;
        return false;
    }
    this->unit()->simplify();

    m_value = log(m_value);
    return true;
}

bool PhyxVariable::mathLog10()
{
    if (!this->unit()->isDimensionlessUnit())
    {
        m_error = PhyxVariable::UnitNotDimensionlessError;
        return false;
    }
    this->unit()->simplify();

    m_value = log10(m_value);
    return true;
}

bool PhyxVariable::mathLog2()
{
    if (!this->unit()->isDimensionlessUnit())
    {
        m_error = PhyxVariable::UnitNotDimensionlessError;
        return false;
    }
    this->unit()->simplify();

    m_value = log2(m_value);
    return true;
}

bool PhyxVariable::mathLogn(PhyxVariable *variable)
{
    if (!this->unit()->isDimensionlessUnit() || !variable->unit()->isDimensionlessUnit())
    {
        m_error = PhyxVariable::UnitNotDimensionlessError;
        return false;
    }
    this->unit()->simplify();
    variable->unit()->simplify();

    m_value = log(this->value()) / log(variable->value());
    return true;
}

void PhyxVariable::mathAbs()
{
    m_value = qAbs(m_value);
}

bool PhyxVariable::mathMax(PhyxVariable *variable)
{
    if (this->unit()->isSame(variable->unit()))
    {
        this->m_value = qMax(this->value(), variable->value());
        return true;
    }
    else if (this->unit()->isConvertible(variable->unit()))
    {
        this->unit()->compoundsEqualize(variable->unit());

        this->m_value = qMax(this->value(), variable->value());
        return true;
    }
    else
    {
        this->m_error = UnitsNotConvertibleError;
        return false;
    }
}

bool PhyxVariable::mathMin(PhyxVariable *variable)
{
    if (this->unit()->isSame(variable->unit()))
    {
        this->m_value = qMax(this->value(), variable->value());
        return true;
    }
    else if (this->unit()->isConvertible(variable->unit()))
    {
        this->unit()->compoundsEqualize(variable->unit());

        this->m_value = qMin(this->value(), variable->value());
        return true;
    }
    else
    {
        this->m_error = UnitsNotConvertibleError;
        return false;
    }
}

void PhyxVariable::mathInt()
{
    m_value = (int)m_value;
}

void PhyxVariable::mathTrunc()
{
    m_value = trunc(m_value);
}

void PhyxVariable::mathFloor()
{
    m_value = floor(m_value);
}

void PhyxVariable::mathRound()
{
    m_value = round(m_value);
}

void PhyxVariable::mathCeil()
{
    m_value = ceil(m_value);
}

void PhyxVariable::mathSign()
{
    m_value = copysign(1.0,m_value);
}

bool PhyxVariable::mathHeaviside()
{
    if (!this->unit()->isDimensionlessUnit())
    {
        m_error = PhyxVariable::UnitNotDimensionlessError;
        return false;
    }
    this->unit()->simplify();

    if (m_value >= 0)
        m_value = 1;
    else
        m_value = 0;

    return true;
}

bool PhyxVariable::mathRandg(PhyxVariable *variable)
{
    if (!this->unit()->isDimensionlessUnit() || !variable->unit()->isDimensionlessUnit())
    {
        m_error = PhyxVariable::UnitNotDimensionlessError;
        return false;
    }
    this->unit()->simplify();
    variable->unit()->simplify();

    int min = this->value();
    int max = variable->value();
    m_value = min + qrand()%(max-min+1);

    return true;
}

bool PhyxVariable::mathFaculty()
{
    if (!this->unit()->isDimensionlessUnit())
    {
        m_error = PhyxVariable::UnitNotDimensionlessError;
        return false;
    }
    this->unit()->simplify();

    PhyxValueDataType value = m_value;

    if (value < 0)
    {
        m_error = PhyxVariable::ValueNotPositiveError;
        return false;
    }
    if ((int)value != value)
    {
        m_error = PhyxVariable::ValueNotIntegerError;
        return false;
    }

    int n = (int)round(value);
    value = 1;
    for (int i = 2; i <=n; i++)
        value *= i;

    m_value = value;

    return true;
}

void PhyxVariable::setUnit(PhyxUnit *unit)
{
    m_unit->fromSimpleUnit(unit);
}