#include "eSlayerHelpers/eslayerquests.h"

#include "eSlayerHelpers/equests.h"
#include "eSlayerHelpers/epacket.h"

bool eSlayerQuests::npcVisible(
    const uint8_t npcId) const {
    const auto& map = eQuests::sNPCVisibility;
    const auto it = map.find(npcId);
    if(it == map.end()) return true;
    for(const auto& vis : it->second) {
        const auto questId = vis.fQuestId;
        const auto s = stage(questId);
        switch(vis.fType) {
        case eQuestNPCVisibilityType::appear:
            return s > vis.fStageId;
        case eQuestNPCVisibilityType::disappear:
            return s <= vis.fStageId;
        }
    }
    return true;
}

bool eSlayerQuests::npcHiddenInFuture(
    const uint8_t npcId) const {
    const auto& map = eQuests::sNPCVisibility;
    const auto it = map.find(npcId);
    if(it == map.end()) return true;
    for(const auto& vis : it->second) {
        const auto questId = vis.fQuestId;
        const auto s = stage(questId);
        switch(vis.fType) {
        case eQuestNPCVisibilityType::appear:
            break;
        case eQuestNPCVisibilityType::disappear:
            return s <= vis.fStageId;
        }
    }
    return false;
}

bool eSlayerQuests::npcAllowHire(const uint8_t npcId) const {
    const auto& map = eQuests::sNPCAllowHire;
    const auto it = map.find(npcId);
    if(it == map.end()) return true;
    for(const auto& vis : it->second) {
        const auto questId = vis.fQuestId;
        return finished(questId);
    }
    return true;
}

bool eSlayerQuests::hasQuest(const uint8_t questId) const {
    const auto it = mQuests.find(questId);
    return it != mQuests.end();
}

uint8_t eSlayerQuests::stage(
    const uint8_t questId) const {
    const bool r = hasQuest(questId);
    if(!r) return 0;
    return mQuests.at(questId).fStage;
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
    mQuests.clear();
    for(const auto& it : eQuests::sQuests) {
        const auto id = it.fId;
        const auto& q = it.fValue;
        eQuestState qs;

        auto& ss = qs.fStages;
        ss.emplace_back(); // intro
        for(const auto& s : q.fSteps) {
            ss.emplace_back();
        }
        ss.emplace_back(); // outro

        mQuests.emplace(id, qs);
    }
}

bool eSlayerQuests::nextStage(
    const uint8_t questId) {
    if(finished(questId)) return false;
    auto it = mQuests.find(questId);
    if(it == mQuests.end()) return false;
    auto& s = it->second;
    s.fStage++;
    if(finished(questId)) {
        const auto& quest = eQuests::sQuests.get(questId);
        if(quest.fFinishDifficulty) {
            mDifficultyFinished = true;
        }
        mSkillPoints += quest.fFinishSkillPoints;
        mStatPoints += quest.fFinishStatPoints;
    } else {
        tryProgressQuest(questId);
    }
    mState++;
    return true;
}

bool eSlayerQuests::setStage(
    const uint8_t questId,
    const uint8_t stageId) {
    auto& q = mQuests[questId];
    q.fStage = stageId;
    return true;
}

uint8_t eSlayerQuests::count(
    const uint8_t questId,
    const uint8_t stage) const {
    auto it = mQuests.find(questId);
    if(it == mQuests.end()) return false;
    auto& qq = it->second;
    const auto& s = qq.fStages[stage];
    return s.fCount;
}

bool eSlayerQuests::incCount(
    const uint8_t questId,
    const uint8_t stage,
    uint8_t* const count) {
    const auto c = eSlayerQuests::count(questId, stage);
    if(count) *count = c + 1;
    return setCount(questId, stage, c + 1);
}

bool eSlayerQuests::setCount(
    const uint8_t questId,
    const uint8_t stage,
    const uint8_t count) {
    const auto& q = eQuests::sQuests.get(questId);

    auto it = mQuests.find(questId);
    if(it == mQuests.end()) return false;
    auto& qq = it->second;

    const bool questIntro = q.introStage(stage);
    const bool questOutro = q.outroStage(stage);
    if(questIntro || questOutro) {
        if(qq.fStage != stage) return false;
        return nextStage(questId);
    }
    const auto stepId = q.stageToStep(stage);
    if(stepId >= q.fSteps.size()) return false;
    const auto& step = q.fSteps[stepId];

    if(!step.fAlwaysTrack) {
        if(qq.fStage != stage) return false;
    }
    auto& s = qq.fStages[stage];
    s.fCount = count;

    return tryProgressQuest(questId);
}

bool eSlayerQuests::isAddSocketStage(
    const uint8_t questId) const {
    const auto it = mQuests.find(questId);
    if(it == mQuests.end()) return false;
    const auto& s = it->second;
    const auto& q = eQuests::sQuests.get(questId);
    const auto stepId = q.stageToStep(s.fStage);
    const auto& step = q.fSteps[stepId];
    return step.fType == eQuestType::addSocket;
}

uint8_t eSlayerQuests::receiveStatPoints() {
    const auto result = mStatPoints;
    mStatPoints = 0;
    return result;
}

uint8_t eSlayerQuests::receiveSkillPoints() {
    const auto result = mSkillPoints;
    mSkillPoints = 0;
    return result;
}

void eSlayerQuests::read(ePacket& p) {
    p >> mState;
    p >> mDifficultyFinished;
    initialize();
    for(auto& it : mQuests) {
        auto& qs = it.second;
        qs.read(p);
    }
}

void eSlayerQuests::write(ePacket& p) const {
    p << mState;
    p << mDifficultyFinished;
    for(const auto& it : mQuests) {
        const auto& qs = it.second;
        qs.write(p);
    }
}

bool eSlayerQuests::tryProgressQuest(
    const uint8_t questId) {
    auto it = mQuests.find(questId);
    if(it == mQuests.end()) return false;
    const auto& qq = it->second;
    const auto stage = qq.fStage;

    const auto& q = eQuests::sQuests.get(questId);
    const auto stepId = q.stageToStep(stage);
    if(stepId >= q.fSteps.size()) return false;
    const auto& step = q.fSteps[stepId];

    const auto& s = qq.fStages[stage];

    if(step.fCount <= s.fCount) {
        return nextStage(questId);
    } else {
        return false;
    }
}

void eQuestState::read(ePacket& p) {
    p >> fStage;
    for(auto& s : fStages) {
        p >> s;
    }
}

void eQuestState::write(ePacket& p) const {
    p << fStage;
    for(const auto& s : fStages) {
        p << s;
    }
}
