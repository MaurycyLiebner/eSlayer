#include "eSlayerHelpers/eequipmentplace.h"

#include "eSlayerHelpers/eexceptions.h"

#include <cstring>
#include <map>

const std::map<ePlaceType, std::string>
    gPlaceTypeToString = {
        {ePlaceType::none, "none"},
        {ePlaceType::boots, "boots"},
        {ePlaceType::gloves, "gloves"},
        {ePlaceType::helmet, "helmet"},
        {ePlaceType::armor, "armor"},
        {ePlaceType::belt, "belt"},
        {ePlaceType::ringL, "ringL"},
        {ePlaceType::ringR, "ringR"},
        {ePlaceType::amulet, "amulet"},
        {ePlaceType::weapon1L, "weapon1L"},
        {ePlaceType::weapon1R, "weapon1R"},
        {ePlaceType::weapon2L, "weapon2L"},
        {ePlaceType::weapon2R, "weapon2R"},
        {ePlaceType::dragged, "dragged"},
        {ePlaceType::inventory, "inventory"},
        {ePlaceType::stash, "stash"},
        {ePlaceType::beltPotions, "beltPotions"},
        {ePlaceType::beltHiddenPotions, "beltHiddenPotions"},
        {ePlaceType::temporary, "temporary"}
};

const std::map<std::string, ePlaceType>
    gStringToPlaceType = [] {
        std::map<std::string, ePlaceType> m;
        for(const auto& [key, value] : gPlaceTypeToString) {
            m[value] = key;
        }
        return m;
    }();

const std::string& ePlaceTypeHelpers::name(const ePlaceType type) {
    const auto it = gPlaceTypeToString.find(type);
    if(it == gPlaceTypeToString.end()) {
        const auto typeStr = std::to_string(static_cast<uint8_t>(type));
        eRuntimeThrow("Unrecognized place type \"" + typeStr + "\".");
    }
    return it->second;
}

ePlaceType ePlaceTypeHelpers::type(const std::string& name) {
    const auto it = gStringToPlaceType.find(name);
    if(it == gStringToPlaceType.end()) {
        eRuntimeThrow("Unrecognized place type \"" + name + "\".");
    }
    return it->second;
}
