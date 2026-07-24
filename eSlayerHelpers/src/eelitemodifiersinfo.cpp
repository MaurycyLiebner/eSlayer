#include "eSlayerHelpers/eelitemodifiersinfo.h"

#include "eSlayerHelpers/efileloaderbase.h"

eStringIdMapVector<eEliteInfo>
eEliteModifiersInfo::sElite;
const int eEliteModifiersInfo::sMaxUnitLevel = 99;
bool eEliteModifiersInfo::sLoaded = false;

void eEliteModifiersInfo::load() {
    if(sLoaded) return;
    sLoaded = true;

    const auto dir = "Units";

    std::vector<std::string> elites;
    try {
        const auto jdata = eFileLoaderBase::parse(dir, "Elite/modifiers.json");
        elites = jdata.get<std::vector<std::string>>();
    } catch(...) {
        eRuntimeThrow("Failed to parse " + dir + "/Elite/modifiers.json");
    }

    sElite.reserve(elites.size() + 1);

    sElite.add("minion", eEliteInfo());

    for(const auto& name : elites) {
        try {
            eEliteInfo elite;
            const auto jdata = eFileLoaderBase::parse(dir, "Elite/" + name + ".json");
            if(jdata.contains("minions")) {
                const auto& minions = jdata["minions"];
                eModsCollectionLevel::parseLevels(
                    minions, elite.fMinions, sMaxUnitLevel, true);
            } else {
                const ordered_json empty = ordered_json::object();
                eModsCollectionLevel::parseLevels(
                    empty, elite.fMinions, sMaxUnitLevel, true);
            }
            if(jdata.contains("boss")) {
                const auto& boss = jdata["boss"];
                eModsCollectionLevel::parseLevels(
                    boss, elite.fBoss, sMaxUnitLevel, true);
            } else {
                const ordered_json empty = ordered_json::object();
                eModsCollectionLevel::parseLevels(
                    empty, elite.fBoss, sMaxUnitLevel, true);
            }
            elite.fBossColorMod = eColor{1.f, 1.f, 1.f, 1.f};
            if(jdata.contains("bossColorMod")) {
                const auto& color = jdata["bossColorMod"];
                elite.fBossColorMod.fR = color.value("r", 1.f);
                elite.fBossColorMod.fG = color.value("g", 1.f);
                elite.fBossColorMod.fB = color.value("b", 1.f);
                elite.fBossColorMod.fA = color.value("a", 1.f);
            }
            sElite.add(name, elite);
        } catch(...) {
            eRuntimeThrow("Failed to parse elite modifiers \"" + name + "\".");
        }
    }
}
