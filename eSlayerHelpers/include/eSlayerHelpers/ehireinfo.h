#ifndef EHIREINFO_H
#define EHIREINFO_H

#include "emercenary.h"

struct eHireInfo : public eMercenaryBase {
    uint32_t fCost = 0;
};

class ESLAYERHELPERS_API eHireInfos {
public:
    static std::vector<eHireInfo> generate(
        const std::vector<uint8_t> mtypes,
        const uint8_t level,
        const uint8_t count);
};

#endif // EHIREINFO_H
