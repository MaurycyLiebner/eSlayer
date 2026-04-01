#include "eSlayerHelpers/eitemrarity.h"

#include <map>

std::map<eItemRarity, std::string>
gRarityTypeToString = {
    {eItemRarity::normal, "normal"},
    {eItemRarity::magic, "magic"},
    {eItemRarity::rare, "rare"},
    {eItemRarity::set, "set"},
    {eItemRarity::unique, "unique"}
};

std::map<std::string, eItemRarity>
gStringToRarityType = [] {
    std::map<std::string, eItemRarity> m;
    for(const auto& [key, value] : gRarityTypeToString) {
        m[value] = key;
    }
    return m;
}();

std::string eItemRarityHelpers::name(const eItemRarity r) {
    return gRarityTypeToString[r];
}

eItemRarity eItemRarityHelpers::type(const std::string& name) {
    return gStringToRarityType[name];
}
