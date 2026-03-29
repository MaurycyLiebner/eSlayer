#ifndef ELOADTEXTHELPER_H
#define ELOADTEXTHELPER_H

#include <string>
#include <map>
#include <vector>

namespace eLoadTextHelper {
    using eMap = std::map<std::string, std::string>;
    bool load(const std::string& path, eMap& map);
    bool load(const std::vector<std::byte>& data, eMap& map);
};

#endif // ELOADTEXTHELPER_H
