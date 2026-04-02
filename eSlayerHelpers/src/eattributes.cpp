#include "eSlayerHelpers/eattributes.h"

#include "eSlayerHelpers/epacket.h"
#include "eSlayerHelpers/efileloaderbase.h"

#include <nlohmann/json.hpp>
using namespace nlohmann;

#include <cmath>

uint8_t eAttributes::sMaxLevel = 99;
std::vector<uint32_t>
eAttributes::sLevelExperience;

uint32_t eAttributes::nextLevelExp() const {
    if(fLevel == 0) return 0;
    const auto id = fLevel - 1;
    if(id >= sLevelExperience.size()) return 0;
    return sLevelExperience.at(id);
}

void eAttributes::read(ePacket& p) {
    p >> fLevel;
    p >> fExp;
    p >> fStrength;
    p >> fDexterity;
    p >> fVitality;
    p >> fEnergy;
}

void eAttributes::write(ePacket& p) const {
    p << fLevel;
    p << fExp;
    p << fStrength;
    p << fDexterity;
    p << fVitality;
    p << fEnergy;
}

void eAttributes::load() {
    const auto dir = "Other";
    const auto jdata = eFileLoaderBase::parse(dir, "experience.json");
    sMaxLevel = jdata.value("maxLevel", 99);
    sLevelExperience.resize(sMaxLevel - 1);
    for(int i = 1; i < sMaxLevel; i++) {
        const auto iStr = std::to_string(i);
        sLevelExperience[i - 1] = jdata.value(iStr, sLevelExperience[i - 2]);
    }
}
