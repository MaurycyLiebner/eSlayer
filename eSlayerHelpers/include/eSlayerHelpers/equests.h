#ifndef EQUESTS_H
#define EQUESTS_H

#include "estringidmapvector.h"

#include <cstdint>
#include <optional>

enum class eQuestType {
    kill,
    findItem,
    bringItem,
    getItem,
    talkTo,
    addSocket,
    enterArea
};

enum class eQuestNPCVisibilityType {
    appear, disappear
};

struct eQuestNPCVisibility {
    eQuestNPCVisibilityType fType;
    uint8_t fNPCId;

    uint8_t fQuestId;
    uint8_t fStageId;
};

struct eQuestNPCAllowHire {
    uint8_t fNPCId;

    uint8_t fQuestId;
};

struct eQuestStepId {
    eQuestStepId(const uint8_t qid,
                 const uint8_t sid) :
        fQuestId(qid), fStageId(sid) {}

    uint8_t fQuestId;
    uint8_t fStageId;
};

struct eQuestStep {
    std::string fConvoStr;
    eQuestType fType;
    uint8_t fTargetNPC = 0;
    uint8_t fTargetItem = 0;
    uint8_t fTargetAreaId = 0;
    std::vector<uint8_t> fTargetMonsters;
    uint8_t fCount = 0;
    float fItemWorth = 0.f;
    bool fAlwaysTrack = true;
    bool fAllClientsFulfill = false;

    std::vector<eQuestNPCVisibility> fNPCVisilibty;
};

struct ESLAYERHELPERS_API eQuest {
    uint8_t fAct = 0;
    bool fFinishDifficulty = false;
    std::optional<uint8_t> fAllowsHire;
    std::vector<eQuestStep> fSteps;
    std::vector<uint8_t> fPrerequisites;

    uint8_t fFinishStatPoints = 0;
    uint8_t fFinishSkillPoints = 0;

    uint8_t nStages() const {
        return fSteps.size() + 2;
    }

    bool introStage(const uint8_t stage) const {
        return stage == 0;
    }

    bool outroStage(const uint8_t stage) const {
        return stage == nStages() - 1;
    }

    static uint8_t stageToStep(const uint8_t stage) {
        return stage - 1;
    }

    static uint8_t stepToStage(const uint8_t step) {
        return step + 1;
    }
};

class ESLAYERHELPERS_API eQuests {
public:
    static eStringIdMapVector<eQuest> sQuests;
    static std::map<int, std::vector<eQuestStepId>>
    sKillMonsterQuests;
    static std::map<int, std::vector<eQuestStepId>>
    sFindItemQuests;
    static std::map<int, std::vector<eQuestStepId>>
    sBringItemQuests;
    static std::map<int, std::vector<eQuestStepId>>
    sEnterAreaQuests;
    static std::map<uint8_t, std::vector<eQuestNPCVisibility>>
    sNPCVisibility;
    static std::map<uint8_t, std::vector<eQuestNPCAllowHire>>
    sNPCAllowHire;

    static void load();
private:
    static bool sLoaded;
};

#endif // EQUESTS_H
