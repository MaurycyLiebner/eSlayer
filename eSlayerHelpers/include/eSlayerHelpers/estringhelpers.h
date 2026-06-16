#ifndef ESTRINGHELPERS_H
#define ESTRINGHELPERS_H

#include "eslayerhelpersexport.h"

#include <cstdint>
#include <string>

namespace eStringHelpers {
    ESLAYERHELPERS_API
    std::string replaceAll(std::string str,
                           const std::string& from,
                           const std::string& to);

    ESLAYERHELPERS_API
    std::string floatToString(const float value);
    ESLAYERHELPERS_API
    std::string floatToStringWithSign(const float value);
    ESLAYERHELPERS_API
    std::string toRoman(const uint8_t n);
    ESLAYERHELPERS_API
    uint8_t fromRoman(const std::string& r);
}

#endif // ESTRINGHELPERS_H
