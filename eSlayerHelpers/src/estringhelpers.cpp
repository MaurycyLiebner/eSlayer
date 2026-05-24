#include "eSlayerHelpers/estringhelpers.h"

#include <sstream>

std::string eStringHelpers::replaceAll(std::string str,
                                       const std::string& from,
                                       const std::string& to) {
    auto&& pos = str.find(from, size_t{});
    while (pos != std::string::npos) {
        str.replace(pos, from.length(), to);
        pos = str.find(from, pos + to.length());
    }
    return str;
}

std::string eStringHelpers::floatToString(const float value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

std::string eStringHelpers::floatToStringWithSign(const float value) {
    const auto str = floatToString(value);
    return value >= 0.f ? "+" + str : str;
}
