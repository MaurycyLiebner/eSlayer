#include "eSlayerHelpers/eslayerquests.h"

#include "eSlayerHelpers/equests.h"
#include "eSlayerHelpers/epacket.h"

bool eSlayerQuests::hasQuest(
    const uint8_t questId) const {
    const auto it = mStages.find(questId);
    return it != mStages.end();
}

uint8_t eSlayerQuests::stage(
    const uint8_t questId) const {
    const bool r = hasQuest(questId);
    if(!r) return 0;
    return mStages.at(questId);
}

bool eSlayerQuests::finished(
    const uint8_t questId) const {
    const auto s = stage(questId);
    const auto& quest = eQuests::sQuests.get(questId);
    const uint8_t nstages = quest.nStages();
    return s >= nstages;
}

void eSlayerQuests::initialize() {
    mStages.clear();
    for(const auto& q : eQuests::sQuests) {
        setStage(q.fId, 0);
    }
}

bool eSlayerQuests::addQuest(
    const uint8_t questId) {
    const bool r = hasQuest(questId);
    if(r) return false;
    mStages[questId] = 0;
    return true;
}

bool eSlayerQuests::nextStage(
    const uint8_t questId) {
    auto it = mStages.find(questId);
    if(it == mStages.end()) return false;
    it->second++;
    mState++;
    return true;
}

bool eSlayerQuests::setStage(
    const uint8_t questId,
    const uint8_t stageId) {
    mStages[questId] = stageId;
    return true;
}

void eSlayerQuests::read(ePacket& p) {
    mStages.clear();
    p >> mState;
    uint8_t n;
    p >> n;
    for(int i = 0; i < n; i++) {
        uint8_t questId;
        p >> questId;
        uint8_t stage;
        p >> stage;
        mStages[questId] = stage;
    }
}

void eSlayerQuests::write(ePacket& p) const {
    p << mState;
    const uint8_t n = mStages.size();
    p << n;
    for(const auto& it : mStages) {
        p << it.first;
        p << it.second;
    }
}