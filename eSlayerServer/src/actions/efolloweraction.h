#ifndef EFOLLOWERACTION_H
#define EFOLLOWERACTION_H

#include "eunitbaseaction.h"

class eFollowerAction : public eUnitBaseAction {
public:
    eFollowerAction(eServerUnit& unit,
                    eServerArea& area,
                    const std::shared_ptr<eServerUnit>& follow);

    void increment(const float by) override;
protected:
    void decide() override;
private:
    void teleportTo(eServerUnit& follow);
    void moveTo(eServerUnit& follow);

    bool mMoving = false;
    std::weak_ptr<eServerUnit> mFollow;
};

#endif // EFOLLOWERACTION_H
