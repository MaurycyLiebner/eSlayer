#include "eSlayerHelpers/etalkheard.h"

#include "eSlayerHelpers/eslayerquests.h"
#include "eSlayerHelpers/eunitsinfo.h"
#include "eSlayerHelpers/eobjectsinfo.h"
#include "eSlayerHelpers/equests.h"
#include "eSlayerHelpers/eequipment.h"

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

void eTalkHeard::justHeard(
    const eConvoId& cid,
    eEquipment& eq) {
    setHeard(cid, true);
    const auto& convo = eTalks::get(cid);
    const auto qid = convo.fQuestId;
    const auto& q = eQuests::sQuests.get(qid);
    const auto stepId = q.stageToStep(convo.fStageId);
    if(stepId > q.nStages()) {
        return;
    }
    const auto& step = q.fSteps[stepId];
    if(step.fType == eQuestType::bringItem) {
        std::vector<uint32_t> items;
        eq.iterateOverAll([&](const eItem& item) {
            if(items.size() >= step.fCount) return;
            if(item.fDataId == step.fTargetItem) {
                items.emplace_back(item.fItemId);
            }
        });
        for(const auto itemId : items) {
            eq.take(itemId);
        }
    }
}

void eTalkHeard::setHeard(
    const eConvoId& cid,
    const bool h) {
    (*this)[cid] = h;
}

std::optional<eConvoId>
eTalkHeard::nextUnheard(
    const std::string& npcName,
    const eSlayerQuests& squests,
    const eEquipment& eq) {
    const int npcId = eTalks::sTalk.id(npcName);
    if(npcId < 0) return std::nullopt;
    return nextUnheard(npcId, squests, eq);
}

std::optional<eConvoId>
eTalkHeard::nextUnheard(
    const uint8_t npcId,
    const eSlayerQuests& squests,
    const eEquipment& eq) {
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
            {
                const auto& q = eQuests::sQuests.get(qid);
                const auto stepId = q.stageToStep(s);
                if(stepId > q.nStages()) {
                    continue;
                }
                const auto& step = q.fSteps[stepId];
                if(step.fType == eQuestType::bringItem) {
                    std::vector<uint32_t> items;
                    eq.iterateOverAll([&](const eItem& item) {
                        if(items.size() >= step.fCount) return;
                        if(item.fDataId == step.fTargetItem) {
                            items.emplace_back(item.fItemId);
                        }
                    });
                    if(items.size() < step.fCount) {
                        continue;
                    }
                }
            }
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
    const eNPC& npc,
    const eSlayerQuests& squests,
    const eEquipment& eq) {
    const auto objectId = npc.fId;
    const auto& map = mNPCWantsToTalk[npc.fType];
    const auto it = map.find(objectId);
    if(it == map.end()) {
        return updateWantsToTalk(
            npc, squests, eq);
    }
    const auto& wt = it->second;
    return wt.fWantsToTalk;
}

bool eTalkHeard::updateWantsToTalk(
    const eNPC& npc,
    const eSlayerQuests& squests,
    const eEquipment& eq) {
    auto& map = mNPCWantsToTalk[npc.fType];

    std::string baseName;
    switch(npc.fType) {
    case eTalkNPCType::object:
        baseName = eObjectsInfo::sObjects.name(npc.fTypeId);
        break;
    case eTalkNPCType::unit:
        baseName = eUnitsInfo::sUnits.name(npc.fTypeId);
        break;
    }

    const auto npcId = eTalks::sTalk.id(baseName);
    if(npcId < 0) {
        return false;
    }
    const auto next = nextUnheard(
        npcId, squests, eq);
    auto& wt = map[npc.fId];
    wt.fNPC = npc;
    wt.fWantsToTalk = !!next;
    return !!next;
}

void eTalkHeard::updateWantsToTalk(
    const eSlayerQuests& squests,
    const eEquipment& eq) {
    for(const auto& map : mNPCWantsToTalk) {
        for(const auto& it : map.second) {
            const auto unitId = it.first;
            const auto& wt = it.second;
            const auto npc = wt.fNPC;
            updateWantsToTalk(npc, squests, eq);
        }
    }
}
