#include "eSlayerHelpers/epotiontype.h"

#include <map>

std::map<ePotionType, std::string>
gPotionTypeToString = {
    {ePotionType::minorHealing, "minorHealing"},
    {ePotionType::lightHealing, "lightHealing"},
    {ePotionType::healing, "healing"},
    {ePotionType::greaterHealing, "greaterHealing"},

    {ePotionType::minorMana, "minorMana"},
    {ePotionType::lightMana, "lightMana"},
    {ePotionType::mana, "mana"},
    {ePotionType::greaterMana, "greaterMana"},

    {ePotionType::minorRejuvenation, "minorRejuvenation"},
    {ePotionType::lightRejuvenation, "lightRejuvenation"},
    {ePotionType::rejuvenation, "rejuvenation"},
    {ePotionType::greaterRejuvenation, "greaterRejuvenation"},
};

std::map<std::string, ePotionType>
gStringToPotionType = [] {
    std::map<std::string, ePotionType> m;
    for(const auto& [key, value] : gPotionTypeToString) {
        m[value] = key;
    }
    return m;
}();

std::string ePotionTypeHelpers::name(const ePotionType type) {
    return gPotionTypeToString[type];
}

ePotionType ePotionTypeHelpers::type(const std::string& name) {
    return gStringToPotionType[name];
}
