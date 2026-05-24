#ifndef ESTRINGHELPERS_H
#define ESTRINGHELPERS_H

#include "eslayerhelpersexport.h"

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
}

#endif // ESTRINGHELPERS_H
