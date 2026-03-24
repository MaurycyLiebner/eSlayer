#include "eserverunit.h"

#include "ecomplexaction.h"

#include <eSlayerHelpers/emovementhandler.h>
#include <eSlayerHelpers/erunsettings.h>

int eServerUnit::sNextCharId = 0;

eServerUnit::eServerUnit(const eCharData& data)
    : mData(data) {}

float eServerUnit::defense() const {
    if(fAnim == mData.runAnimId()) {
        return 0.f;
    } else {
        return mDefense;
    }
}

float eServerUnit::blockChance() const {
    if(fAnim == mData.runAnimId()) {
        return std::clamp(0.33f*mBlockChance, 0.f, 0.25f);
    } else {
        return std::clamp(mBlockChance, 0.f, 0.75f);
    }
}

float eServerUnit::sHitChance(
    const eServerUnit& hit, const eServerUnit& by) {
    const float alvl = by.level();
    const float dlvl = hit.level();
    const float ar = by.attackRating();
    const float dr = hit.defense();
    return std::clamp(2.f*alvl/(alvl + dlvl)*ar/(ar + dr), 0.05f, 0.95f);
}

bool eServerUnit::getHit(const eHitData& data) {
    if(!mAction) return false;
    return mAction->getHit(data);
}

void eServerUnit::increment(const float by) {
    for(auto& it : mCooldowns) {
        it.second = std::max(0.f, it.second - by);
    }
    if(mAction) mAction->increment(by);
    const float tmp = fActionTime;
    fActionTime -= by;
    if(tmp > 0.f) return;
    const bool r = mHandler.increment(by);
    if(r) {
        const auto newPos = mHandler.pos();
        fVel = ePointF::vector(newPos, fPos);
        fAngle = fVel.angle();
        fPos = newPos;
    } else {
        mHandler.stopMoving();
        fVel = eVec2f{0.f, 0.f};
    }
}

bool eServerUnit::skillReady(const int skillId) const {
    const auto it = mCooldowns.find(skillId);
    if(it == mCooldowns.end()) return true;
    return it->second <= 0.f;
}

void eServerUnit::useSkill(const int skillId) {
    const auto& skill = eSkills::sSkills.get(skillId);
    mCooldowns[skillId] = skill.fBaseCooldown*eRunSettings::sFPS;
}

void eServerUnit::setAction(const std::shared_ptr<eComplexAction>& a) {
    mAction = a;
}

void eServerUnit::setChildAction(const std::shared_ptr<eUnitAction>& a) {
    if(mAction) mAction->setChild(a);
}
