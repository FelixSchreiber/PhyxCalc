#include "qearleyparser.h"

QEarleyParser::QEarleyParser(QObject *parent) :
    QObject(parent)
{
}

bool QEarleyParser::loadRule(QString rule)
{
    //fixing the nonTerminal 0 problem, 0 is already a terminal
    if (nonTerminals.isEmpty())
    {
        nonTerminals.append(QString());
        rules.append(QList<EarleyRule>());
        isNullableVector.append(false);
    }

    int equalPos = rule.indexOf('=');
    if (equalPos == -1)
    {
        qDebug() << "Not a valid rule";
        return false;
    }

    //split premise and conclusio
    QString premise = rule.left(equalPos);
    QString conclusio = rule.mid(equalPos+1);

    QList<EarleySymbol> conclusioConverted;
    EarleySymbol premiseConverted;

    premiseConverted = addNonTerminal(premise);         //convert premise

    //convert conclusio
    if (!conclusio.isEmpty())   //check for epsilon rule
    {
        bool isNonTerminal = false;
        int nonTerminalPos;
        for (int i = 0; i < conclusio.size(); i++)
        {
            if (conclusio.at(i) == '|')
            {
                if (!isNonTerminal)
                    nonTerminalPos = i+1;
                else
                {
                    QString tmpNonTerminal = conclusio.mid(nonTerminalPos, i-nonTerminalPos);
                    conclusioConverted.append(addNonTerminal(tmpNonTerminal));
                }
                isNonTerminal = !isNonTerminal;
            }
            else if (!isNonTerminal)
            {
                conclusioConverted.append(conclusio.at(i).unicode());
            }
        }
    }
    else
    {
        isNullableVector[-premiseConverted] = true;     //if epsilon rule, nonTerminal is nullable
    }

    rules[-premiseConverted].append(conclusioConverted);

    return true;
}

bool QEarleyParser::removeRule(QString rule)
{
    int equalPos = rule.indexOf('=');
    if (equalPos == -1)
    {
        qDebug() << "Not a valid rule";
        return false;
    }

    //split premise and conclusio
    QString premise = rule.left(equalPos);
    QString conclusio = rule.mid(equalPos+1);

    QList<EarleySymbol> conclusioConverted;
    EarleySymbol premiseConverted;

    premiseConverted = -nonTerminals.indexOf(premise);      //find premise
    if (premiseConverted == 1)
    {
        qDebug() << "unknown rule";
        return false;
    }

    //convert conclusio
    if (!conclusio.isEmpty())   //check for epsilon rule
    {
        bool isNonTerminal = false;
        int nonTerminalPos;
        for (int i = 0; i < conclusio.size(); i++)
        {
            if (conclusio.at(i) == '|')
            {
                if (!isNonTerminal)
                    nonTerminalPos = i+1;
                else
                {
                    QString         tmpNonTerminal = conclusio.mid(nonTerminalPos, i-nonTerminalPos);
                    EarleySymbol    tmpNTConverted;
                    tmpNTConverted = -nonTerminals.indexOf(tmpNonTerminal);      //find non terminal
                    if (tmpNTConverted == 1)
                    {
                        qDebug() << "unknown rule";
                        return false;
                    }
                    else
                        conclusioConverted.append(tmpNTConverted);
                }
                isNonTerminal = !isNonTerminal;
            }
            else if (!isNonTerminal)
            {
                conclusioConverted.append(conclusio.at(i).unicode());
            }
        }
    }

    //remove the rule
    QList<EarleyRule> ruleList = rules.at(-premiseConverted);
    for (int i = (ruleList.size()-1); i >= 0; i--)
    {
        bool match = true;
        if (ruleList.at(i).size() != conclusioConverted.size())
            match = false;
        else
        {
            for (int i2 = 0; i2 < ruleList.at(i).size(); i2++)
            {
                if (ruleList.at(i).at(i2) != conclusioConverted.at(i2))
                {
                    match = false;
                    break;
                }
            }
        }

        if (match)
        {
            rules.remove(i);
            return true;
        }
    }

    qDebug() << "unknown rule";
    return false;
}

bool QEarleyParser::loadRules(QStringList ruleList)
{
    nonTerminals.clear();
    rules.clear();
    isNullableVector.clear();

    foreach (QString rule, ruleList)
    {
        if (!loadRule(rule))
            return false;
    }

    qDebug() << "non terminals:" << nonTerminals;

    return true;
}

EarleySymbol QEarleyParser::addNonTerminal(QString nonTerminal)
{
    if (nonTerminals.contains(nonTerminal))
        return -nonTerminals.indexOf(nonTerminal);
    else
    {
        nonTerminals.append(nonTerminal);
        rules.append(QList<EarleyRule>());
        isNullableVector.append(false);
        return -(nonTerminals.size()-1);
    }
}

void QEarleyParser::initialize()
{
    itemListCount = word.size() + 1;

    earleyItemLists.clear();
    for (int i = 0; i < itemListCount; i++)
    {
        earleyItemLists.append(EarleyItemList());
    }
}

bool QEarleyParser::parse(int startPosition)
{
    int currentIndex = startPosition;

    //predictor special case
    foreach (EarleyRule rule, rules.at(-startSymbol))
    {
        appendEarleyItem(0, startSymbol, EarleyRule(), rule, 0);
    }

    for (int i = startPosition; i < itemListCount; i++)
    {
        int oldcount = -1;
        while (earleyItemLists.at(currentIndex).size() != oldcount)
        {
            oldcount = earleyItemLists.at(currentIndex).size();

            foreach (EarleyItem item, earleyItemLists.at(currentIndex))
            {
                if (!item.beta.isEmpty())
                {
                    EarleySymbol firstSymbol = item.beta.at(0);
                    if (firstSymbol < 0)    //if symbol < 0, symbol = nonTerminal
                    {
                        //Predictor
                        foreach (EarleyRule rule, rules.at(-firstSymbol))
                        {
                            appendEarleyItem(currentIndex, firstSymbol, EarleyRule(), rule, currentIndex);
                        }
                        //Aycock and Horspool Epsilon solution
                        if (isNullableVector.at(-firstSymbol))  //if B is nullable
                        {
                            EarleyRule newAlpha = item.alpha;
                            EarleyRule newBeta = item.beta;
                            newAlpha.append(newBeta.takeFirst());   //move point right

                            appendEarleyItem(currentIndex, item.A, newAlpha, newBeta, item.K);
                        }
                    }
                    else if (currentIndex < (itemListCount-1))
                    {
                        //Scanner
                        if ((firstSymbol >= 0) && (word.at(currentIndex) == firstSymbol))
                        {
                            EarleyRule newAlpha = item.alpha;
                            EarleyRule newBeta = item.beta;
                            newAlpha.append(newBeta.takeFirst());   //move point right

                            appendEarleyItem(currentIndex+1, item.A, newAlpha, newBeta, item.K);
                        }
                    }
                }
                else
                {
                    //Completer
                    foreach (EarleyItem item2, earleyItemLists.at(item.K))
                    {
                        if (!item2.beta.isEmpty() && (item2.beta.at(0) == item.A))
                        {
                            EarleyRule newAlpha = item2.alpha;
                            EarleyRule newBeta = item2.beta;
                            newAlpha.append(newBeta.takeFirst());   //move point right

                            appendEarleyItem(currentIndex, item2.A, newAlpha, newBeta, item2.K);
                        }
                    }
                }
            }
        }
        currentIndex++;
    }

    //check wheter parsing was successful or not
    return checkSuccessful();
}

bool QEarleyParser::checkSuccessful()
{
    foreach (EarleyItem item, earleyItemLists.last())
    {
        if ((item.A == startSymbol) && item.beta.isEmpty())
            return true;
    }
}

void QEarleyParser::setWord(QString earleyWord)
{
    word.clear();
    foreach (QChar character, earleyWord)
    {
        word.append(character.unicode());
    }

    initialize();
}

void QEarleyParser::setStartSymbol(QString earleyStartSymbol)
{
    startSymbol = -nonTerminals.indexOf(earleyStartSymbol);
}

// this function is only for testing purposes
bool QEarleyParser::parseWord(QString earleyWord)
{
    setWord(earleyWord);
    return parse();
}

void QEarleyParser::clearWord()
{
    itemListCount = 0;
    earleyItemLists.clear();
}

bool QEarleyParser::addSymbol(QChar earleySymbol)
{
    word.append(earleySymbol.unicode());
    itemListCount++;
    earleyItemLists.append(EarleyItemList());

    return parse(itemListCount-2);
}

bool QEarleyParser::removeSymbol()
{
    if (itemListCount > 1)
    {
        itemListCount--;
        earleyItemLists.removeLast();
        return checkSuccessful();
    }
    else
        return false;
}

void QEarleyParser::appendEarleyItem(int index, EarleySymbol A, EarleyRule alpha, EarleyRule beta, int K)
{
    /*bool match = false;
    foreach (EarleyItem item, earleyItemLists.at(index))
    {
        match |= (item.A == A) && (item.alpha == alpha) && (item.beta == beta) && (item.K == K);
        if (match)
            return;
    }*/

    EarleyItem earleyItem;
    earleyItem.A = A;
    earleyItem.alpha = alpha;
    earleyItem.beta = beta;
    earleyItem.K = K;

    if (!earleyItemLists.at(index).contains(earleyItem))
        earleyItemLists[index].append(earleyItem);
    else
        return;

//    qDebug() << index << A << alpha << beta << K;
}

void QEarleyParser::treeRecursion(int listIndex, int itemIndex, EarleyItemList *tree)
{
    if (listIndex != 0)
    {
        if (!tree->at(itemIndex).alpha.isEmpty())
        {
            EarleySymbol lastSymbol = tree->at(itemIndex).alpha.last();
            if (lastSymbol < 0)     //if symbol < 0, symbol = nonTerminal
            {
                //backward predictor
                int k = tree->at(itemIndex).K;
                int p = tree->at(itemIndex).alpha.size();
                for (int i = 0; i < earleyItemLists.at(listIndex).size(); i++)//foreach (EarleyItem item, earleyItemLists.at(listIndex))
                {
                    EarleyItem *item = &earleyItemLists[listIndex][i];
                    if ((item->A == lastSymbol) && (item->K >= k+p-1))  //item.beta.isEmpty() is now unnessecary here
                    {
                        tree->insert(itemIndex+1,*item);
                        treeRecursion(listIndex, itemIndex+1, tree);
                        //earleyItemLists[listIndex].removeAt(i);
                        //i--;
                    }
                }
            }
            else
            {
                //backward scanner
                (*tree)[itemIndex].beta.prepend((*tree)[itemIndex].alpha.takeLast());       //move point left
                treeRecursion(listIndex-1, itemIndex, tree);
            }
        }
        else
        {
            //backward completer
            if (itemIndex > 0)
            {
                if (!tree->at(itemIndex-1).alpha.isEmpty())
                    (*tree)[itemIndex-1].beta.prepend((*tree)[itemIndex-1].alpha.takeLast());
                treeRecursion(listIndex, itemIndex-1, tree);
            }
        }
    }
}

QList<EarleyTreeItem> QEarleyParser::getTree()
{
    EarleyItemList tree;

    //remove unneeded items
    for (int listIndex = 0; listIndex < itemListCount; listIndex++)
    {
        for (int i = earleyItemLists.at(listIndex).size()-1; i >= 0 ; i--)
        {
            if (!earleyItemLists.at(listIndex).at(i).beta.isEmpty())
                earleyItemLists[listIndex].removeAt(i);
        }
    }

    //for testing purposes only
    for (int i = 0; i < itemListCount; i++)
    {
        foreach (EarleyItem item, earleyItemLists.at(i))
        {
            qDebug() << i << item.A << item.alpha << item.beta << item.K;
        }
    }

    //add the final item
    foreach (EarleyItem item, earleyItemLists.last())
    {
        if ((item.A == startSymbol) && item.beta.isEmpty())
            tree.append(item);
    }

    treeRecursion(itemListCount-1,0,&tree);

    /*for (int i = 0; i < tree.size(); i++)
    {
        EarleyItem item = tree.at(i);
         int index = tree.lastIndexOf(item);
        while (index != i)
        {
            tree.removeAt(index);
            index = tree.lastIndexOf(item);
        }
    }*/

    //creating the binary tree for output
    QList<EarleyTreeItem> outputTree;
    foreach (EarleyItem item, tree)
    {
        QString rule;
        rule.append(nonTerminals.at(-item.A));
        rule.append("=");
        item.alpha.append(item.beta);
        foreach (EarleySymbol symbol, item.alpha)
        {
            if (symbol < 0)
                rule.append("|" + nonTerminals.at(-symbol) + "|");
            else
                rule.append(QChar(symbol));
        }

        outputTree.append(EarleyTreeItem());
        outputTree.last().rule = rule;
        outputTree.last().pos = item.K;
    }

    //for testing purposes only
    foreach (EarleyTreeItem item, outputTree)
    {
        qDebug() << item.rule << item.pos;
    }

    return outputTree;
}
