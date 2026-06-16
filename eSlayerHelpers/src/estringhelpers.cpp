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

std::string eStringHelpers::toRoman(const uint8_t n) {
    if(n == 0) return "I";
    if(n == 1) return "II";
    if(n == 2) return "III";
    if(n == 3) return "IV";
    if(n == 4) return "V";
    if(n == 5) return "VI";
    if(n == 6) return "VII";
    if(n == 7) return "VIII";
    if(n == 8) return "IX";
    return "X";
}

uint8_t eStringHelpers::fromRoman(const std::string& r) {
    if(r == "I") return 0;
    if(r == "II") return 1;
    if(r == "III") return 2;
    if(r == "IV") return 3;
    if(r == "V") return 4;
    if(r == "VI") return 5;
    if(r == "VII") return 6;
    if(r == "VIII") return 7;
    if(r == "IX") return 8;
    return 9;
}
