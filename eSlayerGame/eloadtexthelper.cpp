#include "eloadtexthelper.h"

#include <eSlayerHelpers/eexceptions.h>

#include <string>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <filesystem>

bool eLoadTextHelper::load(const std::vector<std::byte>& data, eMap& map) {
    std::string content(reinterpret_cast<const char*>(data.data()), data.size());
    std::istringstream file(content);

    std::string str;
    while(std::getline(file, str)) {
        if(str.empty()) continue;
        if(str.front() == '\r') continue;
        if(str.front() == '\t') continue;
        if(str.front() == ';') continue;
        const auto keyEnd1 = str.find(' ');
        const auto keyEnd2 = str.find('\t');
        const auto keyEnd = std::min(keyEnd1, keyEnd2);
        if(keyEnd == std::string::npos) continue;
        const auto key = str.substr(0, keyEnd);

        const auto valueStart = str.find('"');
        if(valueStart == std::string::npos) continue;
        const auto valueEnd = str.rfind('"');
        if(valueEnd == std::string::npos) continue;
        const auto valueLen = valueEnd - valueStart;
        const auto value = str.substr(valueStart + 1, valueLen - 1);

        map[key] = value;
    }
    return true;
}


bool eLoadTextHelper::load(const std::string& path, eMap& map) {
    std::ifstream file(path);
    if(!file.good()) {
        eExceptions::logError("File missing " + path);
        return false;
    }
    const auto length { std::filesystem::file_size(path) };
    std::vector<std::byte> data(length);
    file.read(reinterpret_cast<char*>(data.data()), static_cast<long>(length));
    return load(data, map);
}
