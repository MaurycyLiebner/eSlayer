#ifndef EATTACKACTION_H
#define EATTACKACTION_H

#include "eunitactionbase.h"

#include <eSlayerHelpers/eskillchoice.h>
#include <eSlayerHelpers/eweaponchoice.h>

#include <memory>

class eSkill;

enum class eAttackType {
    attack, cast
};

class eAttackAction : public eUnitActionBase {
public:
    using eUnitActionBase::eUnitActionBase;

    static std::shared_ptr<eAttackAction> sCreate(
        eServerUnit& unit, eServerArea& area,
        const std::vector<int>& anims,
        const eAttackType type, const eAction& a,
        const int schoice,
        const eWeaponChoice wchoice);
};

#endif // EATTACKACTION_H
