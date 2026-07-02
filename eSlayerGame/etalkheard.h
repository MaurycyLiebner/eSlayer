#ifndef ETALKHEARD_H
#define ETALKHEARD_H

#include <eSlayerHelpers/etalk.h>

#include <map>

class eSlayerQuests;

class eTalkHeard {
public:
    static std::map<eConvoId, bool> sHeard;
    static void initialize();
    static bool heard(const eConvoId& cid);
    static void setHeard(const eConvoId& cid,
                         const bool h);
    static std::optional<eConvoId> nextUnheard(
        const std::string& npcName,
        const eSlayerQuests& squests);
    static std::vector<eConvoId> allRelevant(
        const std::string& npcName,
        const eSlayerQuests& squests);
};

#endif // ETALKHEARD_H
