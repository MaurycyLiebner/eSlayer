#include "eSlayerHelpers/eunitsinfo.h"

#include "eSlayerHelpers/efileloaderbase.h"
#include "eSlayerHelpers/eexceptions.h"
#include "eSlayerHelpers/echardatainfo.h"

bool eUnitsInfo::sLoaded = false;
eStringIdMapVector<eUnitInfo>
eUnitsInfo::sUnits;

void eUnitsInfo::load() {
    if(sLoaded) return;
    sLoaded = true;

    const auto dir = "Units";

    std::vector<std::string> units;
    try {
        const auto jdata = eFileLoaderBase::parse(dir, "units.json");
        units = jdata.get<std::vector<std::string>>();
    } catch(...) {
        eRuntimeThrow("Failed to parse " + dir + "/units.json");
    }

    sUnits.reserve(units.size());
    for(const auto& name : units) {
        try {
            const int oldId = sUnits.id(name);
            if(oldId != -1) {
                eExceptions::showDialog("Duplicate character '" + name + "' in " + dir + "/units.json");
                continue;
            }
            const auto jdata = eFileLoaderBase::parse(dir, name + ".json");

            eUnitInfo u;
            u.fRadius = jdata.value("radius", 0.4f);
            u.fWalkSpeed = jdata.value("walkSpeed", 0.1f);
            u.fRunSpeed = jdata.value("runSpeed", u.fWalkSpeed);
            const auto textures = jdata.value("textures", "none");
            u.fCharData = eCharDataInfo::id(textures);
            sUnits.add(name, u);
        } catch(const std::exception& e) {
            eExceptions::showDialog(e);
        }
    }

    for(const auto& it : eSkills::sSkills) {
        auto& skill = it.fValue;
        skill.fUnitId = sUnits.id(skill.fUnitStr);
    }
}
