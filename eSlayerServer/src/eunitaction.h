#ifndef EUNITACTION_H
#define EUNITACTION_H

class eServerUnit;
class eServerArea;

class eComplexAction;

class eUnitAction {
public:
    eUnitAction(eServerUnit& unit,
                eServerArea& area);

    virtual void increment(const double by) = 0;

    void setParent(eComplexAction* const c);
    void finishAction();
protected:
    eServerUnit& mUnit;
    eServerArea& mArea;

    eComplexAction* mParent = nullptr;
};

#endif // EUNITACTION_H
