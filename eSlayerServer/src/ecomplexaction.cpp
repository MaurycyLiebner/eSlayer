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
        const float hitChance = eServerUnit::sHitChance(*target, mUnit);
        if(eRand::randF() > hitChance) return;
        const float blockChance = target->blockChance();
        if(eRand::randF() < blockChance) {
            const auto a = eBlockAction::sCreate(*target, mArea);
            if(a) target->setChildAction(a);
        } else {
            const int dmg = 10;
            target->fHealth = std::max(0, target->fHealth - dmg);
            if(target->fHealth <= 0) {
                const auto die = std::make_shared<eDieAction>(*target, mArea);
                target->setChildAction(die);
            } else if(dmg >= target->fMaxHealth/12.f) {
                const auto a = eHitRecoveryAction::sCreate(*target, mArea);
                if(a) target->setChildAction(a);
            }
        }
    };
    const auto attack = eAttackAction::sCreate(mUnit, mArea, a);
    if(attack) setChild(attack);
    return attack.get();
}
