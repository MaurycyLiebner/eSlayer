#ifndef EUNITACTION_H
#define EUNITACTION_H

class eServerUnit;
class eServerArea;

class eUnitAction {
public:
    eUnitAction(eServerUnit& unit,
                eServerArea& area);

    virtual void increment(const double by) = 0;
protected:
    eServerUnit& mUnit;
    eServerArea& mArea;
};

#endif // EUNITACTION_H
