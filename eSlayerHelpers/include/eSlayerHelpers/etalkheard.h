#ifndef ETALKHEARD_H
#define ETALKHEARD_H

#include "etalk.h"

#include <map>

class eSlayerQuests;
struct eEquipment;

enum class eTalkNPCType {
    object, unit
};

struct eNPC {
    eTalkNPCType fType;
    uint32_t fId;
    uint16_t fTypeId;
};

class ESLAYERHELPERS_API eTalkHeard :
    private std::map<eConvoId, bool> {
public:
    void initialize();
    bool heard(const eConvoId& cid) const;
    void justHeard(const eConvoId& cid,
                   eEquipment& eq);
    void setHeard(const eConvoId& cid,
                  const bool h);
    std::optional<eConvoId> nextUnheard(
        const std::string& npcName,
        const eSlayerQuests& squests,
        const eEquipment& eq);
    std::optional<eConvoId> nextUnheard(
        const uint8_t npcId,
        const eSlayerQuests& squests,
        const eEquipment& eq);
    std::vector<eConvoId> allRelevant(
        const std::string& npcName,
        const eSlayerQuests& squests);

    bool wantsToTalk(const eNPC& npc,
                     const eSlayerQuests& squests,
                     const eEquipment& eq);
    bool updateWantsToTalk(const eNPC& npc,
                           const eSlayerQuests& squests,
                           const eEquipment& eq);
    void updateWantsToTalk(const eSlayerQuests& squests,
                           const eEquipment& eq);
private:
    struct eNPCWantsToTalk {
        eNPC fNPC;
        bool fWantsToTalk = false;
    };

    using eNPCIdMap = std::map<uint32_t, eNPCWantsToTalk>;

    std::map<eTalkNPCType, eNPCIdMap> mNPCWantsToTalk {
        {eTalkNPCType::object, {}},
        {eTalkNPCType::unit, {}}
    };
};

#endif // ETALKHEARD_H
