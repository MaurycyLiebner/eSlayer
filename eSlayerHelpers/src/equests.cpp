#include "eSlayerHelpers/equests.h"

#include "eSlayerHelpers/efileloaderbase.h"
#include "eSlayerHelpers/eunitsinfo.h"
#include "eSlayerHelpers/eobjectsinfo.h"
#include "eSlayerHelpers/eitemsdata.h"

eStringIdMapVector<eQuest> eQuests::sQuests;
std::map<int, std::vector<eQuestStepId>>
eQuests::sKillMonsterQuests;
std::map<int, std::vector<eQuestStepId>>
eQuests::sFindItemQuests;
std::map<int, std::vector<eQuestStepId>>
eQuests::sBringItemQuests;
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

                if(!value.contains("steps")) {
                    eRuntimeThrow("Missing quest steps in \"" + key + "\".");
                }
                for(const auto& stepData : value["steps"]) {
                    const int stepId = q.fSteps.size();
                    auto& step = q.fSteps.emplace_back();
                    const auto typeStr = stepData.value("type", "");
                    if(typeStr == "kill") {
                        step.fType = eQuestType::kill;
                        const auto monsterStr = stepData.value("monster", "");
                        const int id = eUnitsInfo::sUnits.id(monsterStr);
                        if(id < 0) {
                            eRuntimeThrow("Unrecognized monster type \"" + monsterStr + "\".");
                        }
                        const auto stageId = eQuest::stepToStage(stepId);
                        sKillMonsterQuests[id].emplace_back(questId, stageId);
                        step.fTargetMonster = id;
                        step.fCount = stepData.value("count", 1);
                    } else if(typeStr == "findItem") {
                        step.fType = eQuestType::findItem;
                        const auto itemStr = stepData.value("item", "");
                        const int id = eItemsData::sItems.id(itemStr);
                        if(id < 0) {
                            eRuntimeThrow("Unrecognized item type \"" + itemStr + "\".");
                        }
                        const auto stageId = eQuest::stepToStage(stepId);
                        sFindItemQuests[id].emplace_back(questId, stageId);
                        step.fTargetItem = id;
                        step.fCount = stepData.value("count", 1);
                    } else if(typeStr == "bringItem") {
                        step.fType = eQuestType::bringItem;
                        {
                            const auto itemStr = stepData.value("item", "");
                            const int id = eItemsData::sItems.id(itemStr);
                            if(id < 0) {
                                eRuntimeThrow("Unrecognized item type \"" + itemStr + "\".");
                            }
                            const auto stageId = eQuest::stepToStage(stepId);
                            sBringItemQuests[id].emplace_back(questId, stageId);
                            step.fTargetItem = id;
                        }
                        {
                            const auto npcStr = stepData.value("npc", "");
                            const int id = eObjectsInfo::sObjects.id(npcStr);
                            if(id < 0) {
                                eRuntimeThrow("Unrecognized NPC type \"" + npcStr + "\".");
                            }
                            step.fTargetNPC = id;
                        }
                        step.fCount = stepData.value("count", 1);
                        step.fConvoStr = stepData.value("conversation", "");
                    } else if(typeStr == "talkTo") {
                        step.fType = eQuestType::talkTo;
                        const auto npcStr = stepData.value("npc", "");
                        const int id = eObjectsInfo::sObjects.id(npcStr);
                        if(id < 0) {
                            eRuntimeThrow("Unrecognized NPC type \"" + npcStr + "\".");
                        }
                        step.fTargetNPC = id;
                        step.fConvoStr = stepData.value("conversation", "");
                    } else if(typeStr == "addSocket") {
                        step.fType = eQuestType::addSocket;
                        const auto npcStr = stepData.value("npc", "");
                        const int id = eObjectsInfo::sObjects.id(npcStr);
                        if(id < 0) {
                            eRuntimeThrow("Unrecognized NPC type \"" + npcStr + "\".");
                        }
                        step.fTargetNPC = id;
                    } else {
                        eRuntimeThrow("Unrecognized quest type \"" + typeStr + "\".");
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
