#include "ecomplexaction.h"

#include "eserverunit.h"
#include "eserverarea.h"
#include "eattackaction.h"
#include "ehitrecoveryaction.h"
#include "eblockaction.h"
#include "edieaction.h"

#include <eSlayerHelpers/epoint.h>

void eComplexAction::increment(const float by) {
    if(mChild) {
        mChild->increment(by);
    } else {
        decide();
    }
}

void eComplexAction::setChild(const std::shared_ptr<eUnitAction>& c) {
    mChild = c;
    if(c) c->setParent(this);
}

bool eComplexAction::getHit(const eServerUnit& by) {
    const float hitChance = eServerUnit::sHitChance(mUnit, by);
    if(eRand::randF() > hitChance) return false;
    const float blockChance = mUnit.blockChance();
    if(eRand::randF() < blockChance) {
        const auto a = eBlockAction::sCreate(mUnit, mArea);
        if(a) mUnit.setChildAction(a);
    } else {
        const int dmg = 10;
        mUnit.fHealth = std::max(0, mUnit.fHealth - dmg);
        if(mUnit.fHealth <= 0) {
            const auto die = std::make_shared<eDieAction>(mUnit, mArea);
            mUnit.setChildAction(die);
        } else if(dmg >= mUnit.fMaxHealth/12.f) {
            const auto a = eHitRecoveryAction::sCreate(mUnit, mArea);
            if(a) mUnit.setChildAction(a);
        }
    }
    return true;
}

bool eComplexAction::attack(const eAttackData& target) {
    switch(target.fType) {
    case eAttackTargetType::character: {
        const auto u = mArea.unit(target.fChar);
        if(!u) return false;
        return attack(*u);
    } break;
    case eAttackTargetType::position: {
        auto dir = ePointF::vector(target.fPos, mUnit.fPos);
        dir.normalize(mUnit.fRadius + 0.2f);
        const auto targetPos = mUnit.fPos + dir;
        const auto a = [this, targetPos]() {
            const auto target = mArea.unit(targetPos);
            if(!target) return;
            if(target.get() == &mUnit) return;
            target->getHit(mUnit);
        };
        const auto attack = eAttackAction::sCreate(mUnit, mArea, a);
        if(attack) setChild(attack);
        return attack.get();
    } break;
    case eAttackTargetType::none: {
        return false;
    } break;
    }
    return true;
}

bool eComplexAction::attack(const eServerUnit& u) {
    if(u.fHealth <= 0) return false;
    const float dist = ePointF::distance(mUnit.fPos, u.fPos);
    const float attackDist = 0.5f*(mUnit.fRadius + u.fRadius);
    if(dist > attackDist) return false;
    const auto dir = ePointF::vector(u.fPos, mUnit.fPos);
    mUnit.fAngle = dir.angle();
    const int targetId = u.fCharId;
    const auto a = [this, targetId]() {
        const auto target = mArea.unit(targetId);
        if(!target) return;
        target->getHit(mUnit);
    };
    const auto attack = eAttackAction::sCreate(mUnit, mArea, a);
    if(attack) setChild(attack);
    return attack.get();
}
