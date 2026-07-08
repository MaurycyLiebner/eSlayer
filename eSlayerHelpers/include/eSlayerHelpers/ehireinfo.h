#ifndef EHIREINFO_H
#define EHIREINFO_H

#include <cstdint>
#include <vector>

struct eHireInfo {
    uint8_t fMercType;
    uint8_t fNameId;
    uint8_t fLevel;
    uint32_t fCost;
};

class eHireInfos {
public:
    static std::vector<eHireInfo> generate(
        const std::vector<uint8_t> mtypes,
        const uint8_t level,
        const uint8_t count);
};

#endif // EHIREINFO_H
