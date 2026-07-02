#ifndef ESLAYERQUESTS_H
#define ESLAYERQUESTS_H

#include "eslayerhelpersexport.h"

#include <map>
#include <cstdint>

class ePacket;

class ESLAYERHELPERS_API eSlayerQuests {
public:
    uint16_t state() const { return mState; }

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

    void read(ePacket& p);
    void write(ePacket& p) const;
private:
    uint16_t mState = 0;
    std::map<uint8_t, uint8_t> mStages;
};

#endif // ESLAYERQUESTS_H
