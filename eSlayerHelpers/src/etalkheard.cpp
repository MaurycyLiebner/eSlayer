#include "../include/eSlayerHelpers/etalkheard.h"

#include "eSlayerHelpers/eslayerquests.h"
#include "eSlayerHelpers/eobjectsinfo.h"

void eTalkHeard::initialize() {
    clear();
    for(const auto& it : eTalks::sTalk) {
        const uint8_t npcId = it.fId;
        const auto& npc = it.fValue;
        const auto& convos = npc.fConvo;
        for(uint8_t i = 0; i < convos.size(); i++) {
            const eConvoId cid{npcId, i};
            (*this)[cid] = false;
        }
    }
}

bool eTalkHeard::heard(const eConvoId& cid) const {
    const auto it = find(cid);
    if(it == end()) return false;
    return it->second;
}

void eTalkHeard::setHeard(
    const eConvoId& cid,
    const bool h) {
    (*this)[cid] = h;
}

std::optional<eConvoId> eTalkHeard::nextUnheard(
    const std::string& npcName,
    const eSlayerQuests& squests) {
    const int npcId = eTalks::sTalk.id(npcName);
    if(npcId < 0) return std::nullopt;
    return nextUnheard(npcId, squests);
}

std::optional<eConvoId>
eTalkHeard::nextUnheard(
    const uint8_t npcId,
    const eSlayerQuests& squests) {
    for(const auto& it : *this) {
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
    for(const auto& it : *this) {
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

bool eTalkHeard::wantsToTalk(
    const uint16_t objType,
    const uint32_t objectId,
    const eSlayerQuests& squests) {
    const auto it = mNPCWantsToTalk.find(objectId);
    if(it == mNPCWantsToTalk.end()) {
        return updateWantsToTalk(objType, objectId, squests);
    }
    const auto& wt = it->second;
    return wt.fWantsToTalk;
}

bool eTalkHeard::updateWantsToTalk(
    const uint16_t objType,
    const uint32_t objectId,
    const eSlayerQuests& squests) {
    const auto baseName = eObjectsInfo::sObjects.name(objType);
    const auto npcId = eTalks::sTalk.id(baseName);
    if(npcId < 0) {
        return false;
    }

    const auto next = nextUnheard(npcId, squests);
    auto& wt = mNPCWantsToTalk[objectId];
    wt.fObjectType = objType;
    wt.fWantsToTalk = !!next;
    return !!next;
}

void eTalkHeard::updateWantsToTalk(
    const eSlayerQuests& squests) {
    for(const auto& it : mNPCWantsToTalk) {
        const auto objId = it.first;
        const auto& wt = it.second;
        const auto objType = wt.fObjectType;
        updateWantsToTalk(objType, objId, squests);
    }
}
