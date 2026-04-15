#include "eSlayerHelpers/eitemdata.h"

#include <map>

std::map<eItemType, std::string>
gItemTypeToString = {
    {eItemType::boots, "boots"},
    {eItemType::gloves, "gloves"},
    {eItemType::helmet, "helmet"},
    {eItemType::armor, "armor"},
    {eItemType::belt, "belt"},
    {eItemType::ring, "ring"},
    {eItemType::amulet, "amulet"},
    {eItemType::weapon, "weapon"},
    {eItemType::shield, "shield"},
    {eItemType::arrows, "arrows"},
    {eItemType::bolts, "bolts"},
    {eItemType::potion, "potion"}
};

std::map<std::string, eItemType>
gStringToItemType = [] {
    std::map<std::string, eItemType> m;
    for(const auto& [key, value] : gItemTypeToString) {
        m[value] = key;
    }
    return m;
}();

eItemType eItemTypeHelpers::type(const std::string& name) {
    return gStringToItemType[name];
}

std::string eItemTypeHelpers::name(const eItemType type) {
    return gItemTypeToString[type];
}
