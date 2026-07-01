#ifndef ESLAYERQUESTS_H
#define ESLAYERQUESTS_H

#include <map>
#include <cstdint>

class ePacket;

class eSlayerQuests {
public:
    bool hasQuest(const uint8_t questId) const;
    uint8_t stage(const uint8_t questId) const;
    bool finished(const uint8_t questId) const;

    void initialize();

    bool addQuest(const uint8_t questId);
    bool nextStage(const uint8_t questId);
    bool setStage(const uint8_t questId,
                  const uint8_t stageId);

    void read(ePacket& p);
    void write(ePacket& p) const;
private:
    std::map<uint8_t, uint8_t> mStages;
};

#endif // ESLAYERQUESTS_H
