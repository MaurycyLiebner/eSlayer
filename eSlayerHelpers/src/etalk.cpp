#include "eSlayerHelpers/etalk.h"

#include "eSlayerHelpers/equests.h"
#include "eSlayerHelpers/efileloaderbase.h"

eStringIdMapVector<eTalk> eTalks::sTalk;
bool eTalks::sLoaded = false;

void eTalks::load() {
    if(sLoaded) return;
    sLoaded = true;

    const auto dir = "Quests";

    std::vector<std::string> npcs;
    try {
        const auto jdata = eFileLoaderBase::parse(dir, "Talk/talk.json");
        npcs = jdata.get<std::vector<std::string>>();
    } catch(...) {
        eRuntimeThrow("Failed to parse " + dir + "/Talk/talk.json");
    }

    for(const auto& npc : npcs) {
        try {
            eTalk talk;
            const int oldId = sTalk.id(npc);
            if(oldId != -1) {
                eExceptions::showDialog("Duplicate NPC '" + npc + "' in " + dir + "/Talk/talk.json");
                continue;
            }
            const auto jdata = eFileLoaderBase::parse(dir, "Talk/" + npc + ".json");
            for(auto it = jdata.begin(); it != jdata.end(); ++it) {
                const auto& key = it.key();
                const auto& value = it.value();
                auto& convo = talk.fConvo.emplace_back();
                const auto typeStr = value.value("type", "");
                if(typeStr == "intro") {
                    convo.fType = eConvoType::intro;
                } else if(typeStr == "questIntro") {
                    convo.fType = eConvoType::questIntro;
                } else if(typeStr == "questOutro") {
                    convo.fType = eConvoType::questOutro;
                } else if(typeStr == "questStep") {
                    convo.fType = eConvoType::questStep;
                } else {
                    eRuntimeThrow("Unrecognized quest type \"" + typeStr + "\".");
                }

                if(convo.fType != eConvoType::intro) {
                    const auto questStr = value.value("quest", "");
                    const int id = eQuests::sQuests.id(questStr);
                    if(id < 0) {
                        eRuntimeThrow("Unrecognized quest \"" + questStr + "\".");
                    }
                    convo.fQuestId = id;
                }
            }
            sTalk.add(npc, talk);
        } catch(...) {
            eRuntimeThrow("Failed to parse " + dir + "/Talk/" + npc + ".json");
        }
    }
}
