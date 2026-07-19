#include "eSlayerHelpers/emercenaries.h"

#include "eSlayerHelpers/eweaponclass.h"
#include "eSlayerHelpers/efileloaderbase.h"
#include "eSlayerHelpers/eunitsinfo.h"

bool eMercenariesInfo::sLoaded = false;
eStringIdMapVector<eMercenaryInfo>
eMercenariesInfo::sMercs;

std::vector<eModifier> eMercenariesInfo::mods(
    const int mercType, const uint8_t level) {
    std::vector<eModifier> result;
    const auto& m = eMercenariesInfo::sMercs.get(mercType);
    {
        auto& mod = result.emplace_back();
        mod.fType = eModifierType::lifeValue;
        mod.fValue1 = level*m.fLifePerLevel;
    }
    {
        auto& mod = result.emplace_back();
        mod.fType = eModifierType::lifeValue;
        mod.fValue1 = level*m.fLifePerLevel;
    }
    {
        auto& mod = result.emplace_back();
        mod.fType = eModifierType::fireResistance;
        mod.fValue1 = level*m.fResistancePerLevel;
    }
    {
        auto& mod = result.emplace_back();
        mod.fType = eModifierType::coldResistance;
        mod.fValue1 = level*m.fResistancePerLevel;
    }
    {
        auto& mod = result.emplace_back();
        mod.fType = eModifierType::lightningResistance;
        mod.fValue1 = level*m.fResistancePerLevel;
    }
    {
        auto& mod = result.emplace_back();
        mod.fType = eModifierType::poisonResistance;
        mod.fValue1 = level*m.fResistancePerLevel;
    }
    {
        auto& mod = result.emplace_back();
        mod.fType = eModifierType::damageValue;
        mod.fValue1 = level*m.fDamagePerLevel;
        mod.fValue2 = level*m.fDamagePerLevel;
    }
    {
        auto& mod = result.emplace_back();
        mod.fType = eModifierType::damageFire;
        mod.fValue1 = level*m.fFireDamagePerLevel;
        mod.fValue2 = level*m.fFireDamagePerLevel;
    }
    {
        auto& mod = result.emplace_back();
        mod.fType = eModifierType::damageCold;
        mod.fValue1 = level*m.fColdDamagePerLevel;
        mod.fValue2 = level*m.fColdDamagePerLevel;
    }
    {
        auto& mod = result.emplace_back();
        mod.fType = eModifierType::damageLightning;
        mod.fValue1 = level*m.fLightningDamagePerLevel;
        mod.fValue2 = level*m.fLightningDamagePerLevel;
    }
    {
        auto& mod = result.emplace_back();
        mod.fType = eModifierType::defenseValue;
        mod.fValue1 = level*m.fDefensePerLevel;
    }

    const auto& mercL = m.fMods.skillLevel(level);
    for(const auto& it : mercL.fTotalModifiers) {
        result.emplace_back(it.second);
    }

    result.emplace_back();
    return result;
}

void eMercenariesInfo::load() {
    if(sLoaded) return;
    sLoaded = true;

    const auto dir = "Units";

    std::vector<std::string> mercs;
    try {
        const auto jdata = eFileLoaderBase::parse(dir, "Mercenaries/mercenaries.json");
        mercs = jdata.get<std::vector<std::string>>();
    } catch(...) {
        eRuntimeThrow("Failed to parse " + dir + "/Mercenaries/mercenaries.json");
    }

    sMercs.reserve(mercs.size());
    for(const auto& name : mercs) {
        try {
            const auto jdata = eFileLoaderBase::parse(dir, "Mercenaries/" +  name + ".json");

            eMercenaryInfo u;

            const auto unitStr = jdata.value("unit", "");
            const int unitId = eUnitsInfo::sUnits.id(unitStr);
            if(unitId < 0) {
                eRuntimeThrow("Unrecognized unit type \"" + unitStr + "\".");
            }
            u.fUnitType = unitId;

            const auto& eq = jdata.value(
                "equipment", std::vector<std::string>());
            for(const auto& place : eq) {
                const auto type = ePlaceTypeHelpers::type(place);
                u.fEq.fEquipment.emplace_back(type);
            }

            const auto& types = jdata.value(
                "types", std::vector<std::string>());
            for(const auto& name : types) {
                const auto type = eItemTypeHelpers::type(name);
                u.fEq.fTypes.emplace_back(type);
            }

            const auto& weapon = jdata.value(
                "weapon", std::vector<std::string>());
            for(const auto& class_ : weapon) {
                const int classId = eWeaponClasses::sClasses.id(class_);
                if(classId < 0) {
                    eRuntimeThrow("Unrecognized weapon class \"" + class_ + "\".");
                }
                u.fEq.fWeaponClasses.emplace_back(classId);
            }

            const auto& uinfo = eUnitsInfo::sUnits.get(unitId);
            const auto maxUnitLevel = uinfo.fMaxLevel;
            if(jdata.contains("modifiers")) {
                const auto& mods = jdata["modifiers"];
                eModsCollectionLevel::parseLevels(
                    mods, u.fMods, maxUnitLevel);
            } else {
                const ordered_json empty = ordered_json::object();
                eModsCollectionLevel::parseLevels(
                    empty, u.fMods, maxUnitLevel);
            }

            sMercs.add(name, u);
        } catch(...) {
            eRuntimeThrow("Failed to parse \"" + dir + "/Mercenaries/" + name + ".json\"");
        }
    }
}
