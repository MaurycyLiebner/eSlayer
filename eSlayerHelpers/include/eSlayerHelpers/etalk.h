#ifndef ETALK_H
#define ETALK_H

#include "estringidmapvector.h"

#include <cstdint>
#include <optional>

enum class eConvoType {
    intro,
    questIntro, questOutro,
    questStep
};

struct eConvoId {
    uint8_t fNPC;
    uint8_t fConvo;

    bool operator<(const eConvoId& other) const noexcept {
        if(fNPC != other.fNPC) return fNPC < other.fNPC;
        return fConvo < other.fConvo;
    }
};

struct eConvo {
    std::string fName;
    eConvoType fType;
    uint8_t fQuestId = 0;
    uint8_t fStageId = 0;
};

struct eTalk {
    std::vector<eConvo> fConvo;
};

class ESLAYERHELPERS_API eTalks {
public:
    static eStringIdMapVector<eTalk> sTalk;
    static std::optional<eConvoId>
    id(const std::string& npc,
       const std::string& convo);
    static const eConvo& get(const eConvoId& id);
    static bool has(const eConvoId& id);

    static void load();
private:
    static bool sLoaded;
};

#endif // ETALK_H
