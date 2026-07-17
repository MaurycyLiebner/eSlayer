#include "eSlayerHelpers/emercenaries.h"

#include "eSlayerHelpers/eweaponclass.h"
#include "eSlayerHelpers/efileloaderbase.h"
#include "eSlayerHelpers/eunitsinfo.h"

bool eMercenariesInfo::sLoaded = false;
eStringIdMapVector<eMercenaryInfo>
eMercenariesInfo::sMercs;

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

            sMercs.add(name, u);
        } catch(...) {
            eRuntimeThrow("Failed to parse \"" + dir + "/Mercenaries/" + name + ".json\"");
        }
    }
}
