#include "etalkheard.h"

#include <eSlayerHelpers/equests.h>
#include <eSlayerHelpers/eslayerquests.h>

std::map<eConvoId, bool>
eTalkHeard::sHeard;

void eTalkHeard::initialize() {
    sHeard.clear();
    for(const auto& it : eTalks::sTalk) {
        const uint8_t npcId = it.fId;
        const auto& npc = it.fValue;
        const auto& convos = npc.fConvo;
        for(uint8_t i = 0; i < convos.size(); i++) {
            const eConvoId cid{npcId, i};
            sHeard[cid] = false;
        }
    }
}

bool eTalkHeard::heard(const eConvoId& cid) {
    const auto it = sHeard.find(cid);
    if(it == sHeard.end()) return false;
    return it->second;
}

void eTalkHeard::setHeard(
    const eConvoId& cid,
    const bool h) {
    sHeard[cid] = h;
}

std::optional<eConvoId> eTalkHeard::nextUnheard(
    const std::string& npcName,
    const eSlayerQuests& squests) {
    const int npcId = eTalks::sTalk.id(npcName);
    for(const auto& it : sHeard) {
        const bool h = it.second;
        if(h) continue;
        const auto& cid = it.first;
        if(cid.fNPC != npcId) continue;
        const auto& convo = eTalks::get(cid);
        const auto qid = convo.fQuestId;
        const bool met = squests.prerequisitesMet(qid);
        if(!met) continue;
        switch(convo.fType) {
        case eConvoType::intro:
            return cid;
        case eConvoType::questIntro: {
            const bool r = squests.introStage(qid);
            if(r) return cid;
        } break;
        case eConvoType::questStep: {
            const auto s = squests.stage(qid);
            const bool r = s == convo.fStageId;
            if(r) return cid;
        } break;
        case eConvoType::questOutro: {
            const bool r = squests.outroStage(qid);
            if(r) return cid;
        } break;
        }
    }
    return std::nullopt;
}

std::vector<eConvoId> eTalkHeard::allRelevant(
    const std::string& npcName,
    const eSlayerQuests& squests) {
    std::vector<eConvoId> result;
    const int npcId = eTalks::sTalk.id(npcName);
    for(const auto& it : sHeard) {
        const auto& cid = it.first;
        if(cid.fNPC != npcId) continue;
        const auto& convo = eTalks::get(cid);
        const auto qid = convo.fQuestId;
        const bool met = squests.prerequisitesMet(qid);
        if(!met) continue;
        bool add = false;
        switch(convo.fType) {
        case eConvoType::intro:
            add = true;
            break;
        case eConvoType::questIntro:
            add = !squests.finished(qid);
            break;
        case eConvoType::questStep: {
            const auto s = squests.stage(qid);
            add = s >= convo.fStageId;
        } break;
        case eConvoType::questOutro:
            add = false;
            break;
        }
        if(add) result.emplace_back(cid);
    }
    return result;
}
