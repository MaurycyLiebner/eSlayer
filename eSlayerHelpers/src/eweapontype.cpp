#include "eSlayerHelpers/eweapontype.h"

#include "eSlayerHelpers/eexceptions.h"

#include <cstring>
#include <map>

std::map<eWeaponType, std::string>
gWeaponTypeToString = {
    {eWeaponType::meele, "meele"},
    {eWeaponType::shield, "shield"},
    {eWeaponType::ranged, "ranged"},
    {eWeaponType::throwable, "throwable"}
};

std::map<std::string, eWeaponType>
gStringToWeaponType = [] {
    std::map<std::string, eWeaponType> m;
    for(const auto& [key, value] : gWeaponTypeToString) {
        m[value] = key;
    }
    return m;
}();

std::string eWeaponTypeHelpers::name(const eWeaponType type) {
    return gWeaponTypeToString[type];
}

eWeaponType eWeaponTypeHelpers::type(const std::string& name) {
    const auto it = gStringToWeaponType.find(name);
    if(it == gStringToWeaponType.end()) {
        eRuntimeThrow("Unrecognized weapon type \"" + name + "\".");
    }
    return it->second;
}
