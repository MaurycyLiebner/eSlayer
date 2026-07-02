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
    std::vector<eConvoId> allRelevant(
        const std::string& npcName,
        const eSlayerQuests& squests);
};

#endif // ETALKHEARD_H
