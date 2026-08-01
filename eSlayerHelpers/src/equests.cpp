#include "eSlayerHelpers/equests.h"

#include "eSlayerHelpers/efileloaderbase.h"
#include "eSlayerHelpers/eunitsinfo.h"
#include "eSlayerHelpers/eitemsdata.h"

eStringIdMapVector<eQuest> eQuests::sQuests;
std::map<int, std::vector<eQuestStepId>>
eQuests::sKillMonsterQuests;
std::map<int, std::vector<eQuestStepId>>
eQuests::sFindItemQuests;
std::map<int, std::vector<eQuestStepId>>
eQuests::sBringItemQuests;
std::map<uint8_t, std::vector<eQuestNPCVisibility>>
eQuests::sNPCVisibility;
std::map<uint8_t, std::vector<eQuestNPCAllowHire>>
eQuests::sNPCAllowHire;
bool eQuests::sLoaded = false;

void eQuests::load() {
    if(sLoaded) return;
    sLoaded = true;

    const auto dir = "Quests";

    try {
        const auto jdata = eFileLoaderBase::parse(dir, "quests.json");
        for(auto it = jdata.begin(); it != jdata.end(); ++it) {
            const auto& key = it.key();
            const auto& value = it.value();

            const int act = std::stoi(key);
            for(auto it = value.begin(); it != value.end(); ++it) {
                const auto& key = it.key();
                const auto& value = it.value();
                const int questId = sQuests.size();
                eQuest q;
                q.fAct = act;

                q.fFinishDifficulty = value.value("finishDifficulty", false);

                const auto allowHireStr = value.value("allowHire", "");
                if(!allowHireStr.empty()) {
                    const auto id = eUnitsInfo::sUnits.id(allowHireStr);
                    if(id < 0) {
                        eRuntimeThrow("Unrecognized allow hire \"" + allowHireStr + "\".");
                    }
                    q.fAllowsHire = id;

                    eQuestNPCAllowHire allow;
                    allow.fQuestId = questId;
                    allow.fNPCId = id;
                    sNPCAllowHire[id].emplace_back(allow);
                }

                q.fFinishStatPoints = value.value("finishStatPoints", 0);
                q.fFinishSkillPoints = value.value("finishSkillPoints", 0);

                if(!value.contains("steps")) {
                    eRuntimeThrow("Missing quest steps in \"" + key + "\".");
                }
                for(const auto& stepData : value["steps"]) {
                    const int stepId = q.fSteps.size();
                    const auto stageId = eQuest::stepToStage(stepId);
                    auto& step = q.fSteps.emplace_back();
                    const auto typeStr = stepData.value("type", "");

                    step.fAlwaysTrack = stepData.value("alwaysTrack", true);
                    step.fAllClientsFulfill = stepData.value("allClientsFulfill", false);

                    const auto parseItem = [&]() {
                        const auto itemStr = stepData.value("item", "");
                        const int id = eItemsData::sItems.id(itemStr);
                        if(id < 0) {
                            eRuntimeThrow("Unrecognized item type \"" + itemStr + "\".");
                        }
                        step.fTargetItem = id;
                    };

                    const auto parseNPC = [&]() {
                        const auto npcStr = stepData.value("npc", "");
                        const int id = eUnitsInfo::sUnits.id(npcStr);
                        if(id < 0) {
                            eRuntimeThrow("Unrecognized NPC type \"" + npcStr + "\".");
                        }
                        step.fTargetNPC = id;
                    };

                    step.fCount = stepData.value("count", 1);
                    step.fConvoStr = stepData.value("conversation", "");
                    if(typeStr == "kill") {
                        step.fType = eQuestType::kill;
                        const auto monsterStr = stepData.value("monster", "");
                        const int id = eUnitsInfo::sUnits.id(monsterStr);
                        if(id < 0) {
                            eRuntimeThrow("Unrecognized monster type \"" + monsterStr + "\".");
                        }
                        sKillMonsterQuests[id].emplace_back(questId, stageId);
                        step.fTargetMonster = id;
                    } else if(typeStr == "findItem") {
                        step.fType = eQuestType::findItem;
                        parseItem();
                        const auto id = step.fTargetItem;
                        sFindItemQuests[id].emplace_back(questId, stageId);
                    } else if(typeStr == "bringItem") {
                        step.fType = eQuestType::bringItem;
                        parseItem();

                        const auto id = step.fTargetItem;
                        sBringItemQuests[id].emplace_back(questId, stageId);

                        parseNPC();
                    } else if(typeStr == "getItem") {
                        step.fType = eQuestType::getItem;
                        parseItem();
                        parseNPC();
                        step.fItemWorth = stepData.value("worth", 0.f);
                    } else if(typeStr == "talkTo") {
                        step.fType = eQuestType::talkTo;
                        parseNPC();
                    } else if(typeStr == "addSocket") {
                        step.fType = eQuestType::addSocket;
                        parseNPC();
                    } else {
                        eRuntimeThrow("Unrecognized quest type \"" + typeStr + "\".");
                    }

                    if(stepData.contains("npcVisibility")) {
                        for(const auto& jvis : stepData["npcVisibility"]) {
                            eQuestNPCVisibility vis;
                            const auto typeStr = jvis.value("type", "");
                            if(typeStr == "appear") {
                                vis.fType = eQuestNPCVisibilityType::appear;
                            } else if(typeStr == "disappear") {
                                vis.fType = eQuestNPCVisibilityType::disappear;
                            } else {
                                eRuntimeThrow("Unrecognized NPC visibility type \"" + typeStr + "\".");
                            }
                            const auto npcStr = jvis.value("npc", "");
                            const auto id = eUnitsInfo::sUnits.id(npcStr);
                            if(id < 0) {
                                eRuntimeThrow("Unrecognized NPC \"" + npcStr + "\".");
                            }
                            vis.fNPCId = id;

                            vis.fQuestId = questId;
                            vis.fStageId = stageId;

                            sNPCVisibility[id].emplace_back(vis);
                            step.fNPCVisilibty.emplace_back(vis);
                        }
                    }
                }

                const auto pStrs = value.value(
                    "prerequisites", std::vector<std::string>());
                for(const auto& p : pStrs) {
                    const int id = sQuests.id(p);
                    if(id < 0) {
                        eRuntimeThrow("Unrecognized quest prerequisite \"" + p + "\".");
                    }
                    q.fPrerequisites.emplace_back(id);
                }

                sQuests.add(key, q);
            }
        }
    } catch(...) {
        eRuntimeThrow("Failed to parse " + dir + "/quests.json");
    }
}
