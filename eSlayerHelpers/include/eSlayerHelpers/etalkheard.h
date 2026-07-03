#ifndef ETALKHEARD_H
#define ETALKHEARD_H

#include "etalk.h"

#include <map>

class eSlayerQuests;

class ESLAYERHELPERS_API eTalkHeard :
    private std::map<eConvoId, bool> {
public:
    void initialize();
    bool heard(const eConvoId& cid) const;
    void setHeard(const eConvoId& cid,
                  const bool h);
    std::optional<eConvoId> nextUnheard(
        const std::string& npcName,
        const eSlayerQuests& squests);
    std::optional<eConvoId> nextUnheard(
        const uint8_t npcId,
        const eSlayerQuests& squests);
    std::vector<eConvoId> allRelevant(
        const std::string& npcName,
        const eSlayerQuests& squests);

    bool wantsToTalk(const uint16_t objType,
                     const uint32_t objectId,
                     const eSlayerQuests& squests);
    bool updateWantsToTalk(const uint16_t objType,
                           const uint32_t objectId,
                           const eSlayerQuests& squests);
    void updateWantsToTalk(const eSlayerQuests& squests);
private:
    struct eNPCWantsToTalk {
        uint16_t fObjectType;
        bool fWantsToTalk = false;
    };

    std::map<uint32_t, eNPCWantsToTalk> mNPCWantsToTalk;
};

#endif // ETALKHEARD_H
