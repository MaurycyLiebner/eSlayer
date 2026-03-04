#include "eloadtexthelper.h"

#include <eSlayerHelpers/eexceptions.h>

#include <SDL3/SDL_log.h>

#include <fstream>
#include <algorithm>

bool eLoadTextHelper::load(const std::string& path, eMap& map) {
    std::ifstream file(path);
    if(!file.good()) {
        eExceptions::logError(
            "File missing " + path);
        return false;
    }
    std::string str;
    while(std::getline(file, str)) {
        if(str.empty()) continue;
        if(str.front() == '\r') continue;
        if(str.front() == '\t') continue;
        if(str.front() == ';') {
            if(str.size() < 7) continue;
            if(str.substr(1, 6) != "PHRASE") continue;
        }
        const auto keyEnd1 = str.find(' ');
        const auto keyEnd2 = str.find('\t');
        const auto keyEnd = std::min(keyEnd1, keyEnd2);
        if(keyEnd == std::string::npos) continue;
        const auto key = str.substr(0, keyEnd);

        const auto valueStart = str.find('"');
        if(valueStart == std::string::npos) continue;
        const auto valueEnd = str.find('"', valueStart + 1);
        const auto valueLen = valueEnd - valueStart;
        const auto value = str.substr(valueStart + 1, valueLen - 1);

        map[key] = value;
    }
    return true;
}
