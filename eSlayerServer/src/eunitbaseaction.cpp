#include "eunitbaseaction.h"

#include "eattackaction.h"
#include "emovetoenemyaction.h"
#include "eserverarea.h"
#include "ewaitaction.h"
#include "egethitaction.h"
#include "eblockaction.h"

#include <eSlayerHelpers/echardata.h>
#include <eSlayerHelpers/erand.h>

void eUnitBaseAction::decide() {
    for(const auto& unit : mArea.units()) {
        if(mUnit.fTeamId == unit->fTeamId) continue;
        const double dist = ePointF::distance(mUnit.fPos, unit->fPos);
        if(dist < 0.5*(mUnit.fRadius + unit->fRadius)) {
            attack(*unit);
            return;
        }
    }
    if(eRand::rand() % 2) {
        const auto move = std::make_shared<eMoveToEnemyAction>(mUnit, mArea);
        setChild(move);
    } else {
        const auto wait = eWaitAction::sCreateStand(mUnit, mArea, 100.);
        setChild(wait);
    }
}

void eUnitBaseAction::attack(const eServerUnit& u) {
    const auto dir = ePointF::vector(u.fPos, mUnit.fPos);
    mUnit.fAngle = dir.angle();
    const int targetId = u.fCharId;
    const auto a = [this, targetId]() {
        const auto target = mArea.unit(targetId);
        if(!target) return;
        const double hitChance = eServerUnit::sHitChance(*target, mUnit);
        if(eRand::randF() > hitChance) return;
        const double blockChance = target->blockChance();
        if(eRand::randF() < blockChance) {
            const auto a = eBlockAction::sCreate(*target, mArea);
            if(a) target->setChildAction(a);
        } else {
            const int dmg = 10;
            target->fHealth = std::max(0, target->fHealth - dmg);
            if(target->fHealth <= 0) {

            } else if(dmg >= target->fMaxHealth/12.) {
                const auto a = eGetHitAction::sCreate(*target, mArea);
                if(a) target->setChildAction(a);
            }
        }
    };
    const auto attack = eAttackAction::sCreate(mUnit, mArea, a);
    if(attack) setChild(attack);
}
