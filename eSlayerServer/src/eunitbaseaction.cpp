#include "eunitbaseaction.h"

#include "eattackaction.h"
#include "emovetoenemyaction.h"
#include "eserverarea.h"
#include "ewaitaction.h"
#include "egethitaction.h"

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
        const auto wait = std::make_shared<eWaitAction>(mUnit, mArea);
        wait->setRemTime(100.);
        setChild(wait);
        const bool a = mUnit.aggressive();
        const auto& data = mUnit.data();
        const int naId = data.animId("stand");
        const int aId = data.animId("standReady");
        if(a) {
            if(aId != -1) {
                mUnit.fAnim = aId;
            } else {
                mUnit.fAnim = naId;
            }
        } else {
            if(naId != -1) {
                mUnit.fAnim = naId;
            } else {
                mUnit.fAnim = aId;
            }
        }
        mUnit.fAnimId++;
    }
}

void eUnitBaseAction::attack(const eServerUnit& u) {
    const auto& data = mUnit.data();
    const int a1Id = data.animId("attack1");
    const int a2Id = data.animId("attack2");

    if(a2Id != -1 && eRand::rand() % 2) {
        mUnit.fAnim = a2Id;
    } else {
        mUnit.fAnim = a1Id;
    }
    mUnit.fAnimId++;
    const auto dir = ePointF::vector(u.fPos, mUnit.fPos);
    mUnit.fAngle = dir.angle();
    const auto attack = std::make_shared<eAttackAction>(mUnit, mArea);
    attack->setDuration(data.animFrames(mUnit.fAnim));
    const int targetId = u.fCharId;
    attack->setAction(data.animActionFrame(mUnit.fAnim), [this, targetId]() {
        const auto u = mArea.unit(targetId);
        if(!u) return;
        u->fHealth = std::max(0, u->fMaxHealth - 10);
        if(u->fHealth <= 0) {

        } else {
            if(eRand::rand() % 4) {
                const auto a = eGetHitAction::sCreate(*u, mArea);
                if(a) u->setAction(a);
            }
        }
    });
    setChild(attack);
}
