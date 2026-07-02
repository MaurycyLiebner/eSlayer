#ifndef EQUESTS_H
#define EQUESTS_H

#include "estringidmapvector.h"

#include <cstdint>

enum class eQuestType {
    kill, talkTo
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
    uint8_t fTarget = 0;
    uint8_t fCount = 0;
};

struct ESLAYERHELPERS_API eQuest {
    uint8_t fAct = 0;
    std::vector<eQuestStep> fSteps;
    std::vector<uint8_t> fPrerequisites;

    uint8_t nStages() const {
        return fSteps.size() + 2;
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
    sMonsterQuests;

    static void load();
private:
    static bool sLoaded;
};

#endif // EQUESTS_H
