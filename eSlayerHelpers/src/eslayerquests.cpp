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
    return mStages.at(questId).fStage;
}

bool eSlayerQuests::finished(
    const uint8_t questId) const {
    const auto s = stage(questId);
    const auto& quest = eQuests::sQuests.get(questId);
    const uint8_t nstages = quest.nStages();
    return s >= nstages;
}

bool eSlayerQuests::introStage(
    const uint8_t questId) const {
    const auto s = stage(questId);
    return s == 0;
}

bool eSlayerQuests::outroStage(
    const uint8_t questId) const {
    const auto s = stage(questId);
    const auto& quest = eQuests::sQuests.get(questId);
    const uint8_t nstages = quest.nStages();
    return s == nstages - 1;
}

bool eSlayerQuests::prerequisitesMet(
    const uint8_t questId) const {
    const auto& quest = eQuests::sQuests.get(questId);
    for(const int qid : quest.fPrerequisites) {
        const bool r = finished(qid);
        if(!r) return false;
    }
    return true;
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
    mStages[questId];
    return true;
}

bool eSlayerQuests::nextStage(
    const uint8_t questId) {
    auto it = mStages.find(questId);
    if(it == mStages.end()) return false;
    auto& s = it->second;
    s.fStage++;
    s.fCount = 0;
    mState++;
    return true;
}

bool eSlayerQuests::setStage(
    const uint8_t questId,
    const uint8_t stageId) {
    auto& s = mStages[questId];
    s.fStage = stageId;
    s.fCount = 0;
    return true;
}

bool eSlayerQuests::incCount(
    const uint8_t questId,
    const uint8_t stage) {
    auto it = mStages.find(questId);
    if(it == mStages.end()) return false;
    auto& s = it->second;
    if(s.fStage != stage) return false;
    return setCount(questId, stage, s.fCount + 1);
}

bool eSlayerQuests::resetCount(
    const uint8_t questId,
    const uint8_t stage) {
    auto it = mStages.find(questId);
    if(it == mStages.end()) return false;
    auto& s = it->second;
    if(s.fStage != stage) return false;
    s.fCount = 0;
    return true;
}

bool eSlayerQuests::setCount(
    const uint8_t questId,
    const uint8_t stage,
    const uint8_t count) {
    auto it = mStages.find(questId);
    if(it == mStages.end()) return false;
    auto& s = it->second;
    if(s.fStage != stage) return false;
    s.fCount = count;
    const auto& q = eQuests::sQuests.get(questId);
    const auto stepId = q.stageToStep(stage);
    if(stepId >= q.fSteps.size()) return false;
    const auto& step = q.fSteps[stepId];
    switch(step.fType) {
    case eQuestType::talkTo:
    case eQuestType::addSocket:
        return false;
    default:
        break;
    }
    if(step.fCount <= s.fCount) {
        return nextStage(questId);
    } else {
        return false;
    }
}

int eSlayerQuests::countNeeded(
    const uint8_t questId,
    const uint8_t stage) {
    auto it = mStages.find(questId);
    if(it == mStages.end()) return 0;
    auto& s = it->second;
    if(s.fStage != stage) return 0;
    const auto& q = eQuests::sQuests.get(questId);
    const auto stepId = q.stageToStep(stage);
    if(stepId >= q.fSteps.size()) return 0;
    const auto& step = q.fSteps[stepId];
    return step.fCount;
}

bool eSlayerQuests::heardTalk(
    const eConvoId& talk) {
    const bool r = eTalks::has(talk);
    if(!r) return false;
    const auto& c = eTalks::get(talk);
    if(c.fType == eConvoType::intro) return false;
    auto& s = mStages[c.fQuestId];
    if(s.fStage != c.fStageId) return false;
    return nextStage(c.fQuestId);
}

bool eSlayerQuests::addedSocket(
    const uint8_t questId) {
    const auto it = mStages.find(questId);
    if(it == mStages.end()) return false;
    const auto& s = it->second;
    const auto& q = eQuests::sQuests.get(questId);
    const auto stepId = q.stageToStep(s.fStage);
    const auto& step = q.fSteps[stepId];
    if(step.fType != eQuestType::addSocket) return false;
    return nextStage(questId);
}

void eSlayerQuests::read(ePacket& p) {
    mStages.clear();
    p >> mState;
    uint8_t n;
    p >> n;
    for(int i = 0; i < n; i++) {
        uint8_t questId;
        p >> questId;
        eQuestState state;
        p >> state;
        mStages[questId] = state;
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