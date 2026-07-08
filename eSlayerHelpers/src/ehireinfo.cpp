#include "eSlayerHelpers/ehireinfo.h"

#include "eSlayerHelpers/erand.h"

std::vector<eHireInfo> eHireInfos::generate(
    const std::vector<uint8_t> mtypes,
    const uint8_t level,
    const uint8_t count) {
    std::vector<eHireInfo> result;
    const uint8_t margin = 5u;
    const uint8_t minLevel = (level <= margin) ? 1u : (level - margin);
    const uint8_t maxLevel = (level < 1u) ? 1u : level;
    for(uint8_t i = 0; i < count; i++) {
        auto& info = result.emplace_back();
        info.fMercType = eRand::randomElement(mtypes);
        info.fLevel = eRand::rand(minLevel, maxLevel);
        info.fNameId = eRand::rand();
        info.fCost = 1000*info.fLevel;
    }
    return result;
}
