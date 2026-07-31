#ifndef ESLAYERQUESTS_H
#define ESLAYERQUESTS_H

#include "eslayerhelpersexport.h"

#include <eSlayerHelpers/etalk.h>

#include <map>
#include <cstdint>

class ePacket;

struct eQuestState {
    uint8_t fStage = 0;
    uint8_t fCount = 0;
};

class ESLAYERHELPERS_API eSlayerQuests {
public:
    uint16_t state() const { return mState; }

    bool npcVisible(const uint8_t npcId) const;
    bool npcHiddenInFuture(const uint8_t npcId) const;

    bool npcAllowHire(const uint8_t npcId) const;

    bool hasQuest(const uint8_t questId) const;
    uint8_t stage(const uint8_t questId) const;
    bool finished(const uint8_t questId) const;
    bool introStage(const uint8_t questId) const;
    bool outroStage(const uint8_t questId) const;
    bool prerequisitesMet(const uint8_t questId) const;

    void initialize();

    bool addQuest(const uint8_t questId);
    bool nextStage(const uint8_t questId);
    bool setStage(const uint8_t questId,
                  const uint8_t stageId);
    bool incCount(const uint8_t questId,
                  const uint8_t stage);
    bool resetCount(const uint8_t questId,
                   const uint8_t stage);
    bool setCount(const uint8_t questId,
                  const uint8_t stage,
                  const uint8_t count);
    int countNeeded(const uint8_t questId,
                   const uint8_t stage);
    bool heardTalk(const eConvoId& talk);
    bool addedSocket(const uint8_t questId);

    bool difficultyFinished() const
    { return mDifficultyFinished; }

    uint8_t receiveStatPoints();
    uint8_t receiveSkillPoints();

    void read(ePacket& p);
    void write(ePacket& p) const;
private:
    uint16_t mState = 0;
    bool mDifficultyFinished = false;
    uint8_t mStatPoints = 0;
    uint8_t mSkillPoints = 0;
    std::map<uint8_t, eQuestState> mStages;
};

#endif // ESLAYERQUESTS_H
