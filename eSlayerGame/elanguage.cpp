#include "elanguage.h"

#include <eSlayerHelpers/egamedir.h>

#include <fstream>

eLanguage eLanguage::sInstance;

eLanguage::eLanguage() {}

bool eLanguage::load() {
    return sInstance.loadImpl();
}

const std::string &eLanguage::text(const int g, const int s) {
    return sInstance.textImpl(g, s);
}

const std::string &eLanguage::textImpl(const int g, const int s) {
    const auto git = mText.find(g);
    if(git == mText.end()) {
        printf("Groud id %i out of range.\n", g);
    }
    const auto& group = git->second;
    const auto sit = group.find(s);
    if(sit == group.end()) {
        printf("String id %i out of range.\n", s);
    }
    return sit->second;
}

bool match(const std::string& str, const std::string& line,
           const int oldIndex, int& newIndex) {
    const int ls = line.size();
    const int iMax = str.size();
    newIndex = oldIndex;
    for(int i = 0; i < iMax; i++, newIndex++) {
        const int lineIndex = oldIndex + i;
        if(lineIndex >= ls) return false;
        const auto strC = str[i];
        const auto lineC = line[lineIndex];
        if(strC != lineC) return false;
    }
    return true;
}

void skipSpaces(const std::string& line, int& index) {
    const int ls = line.size();
    if(index >= ls) return;
    while(line[index] == ' ') {
        index++;
        if(index >= ls) break;
    }
}

bool readId(const std::string& line,
            const int oldIndex,
            int& newIndex,
            int& id) {
    const int ls = line.size();
    newIndex = oldIndex;
    if(newIndex >= ls) return false;
    {
        const bool q = line[newIndex++] == '"';
        if(!q) return false;
    }
    if(newIndex >= ls) return false;
    std::string numberStr;
    while(newIndex < ls) {
        const auto c = line[newIndex];
        if(c == '"') break;
        if(!std::isdigit(c)) return false;
        numberStr = numberStr + c;
        newIndex++;
    }
    id = std::stoi(numberStr);
    {
        if(newIndex >= ls) return false;
        const bool q = line[newIndex++] == '"';
        if(!q) return false;
    }
    return true;
}

using eGroup = std::map<int, std::string>;
using eStrings = std::map<int, eGroup>;
bool parse(const std::string& path,
           eStrings& strings) {
    std::ifstream file(path);
    if(!file.good()) {
        printf("File missing %s\n", path.c_str());
        return false;
    }

    eGroup* group = nullptr;
    std::string line;
    while(std::getline(file, line)) {
        if(line.empty()) continue;
        const int ls = line.size();
        int index = 0;
        {
            skipSpaces(line, index);
            if(index >= ls) continue;
        }
        {
            int newIndex;
            const bool isGroup = match("<group id=", line, index, newIndex);
            if(isGroup) {
                index = newIndex;
                if(index >= ls) continue;
                int id;
                const bool r = readId(line, index, newIndex, id);
                if(!r) continue;
                index = newIndex;
                if(index >= ls) continue;
                group = &strings[id];
                continue;
            }
        }
        {
            int newIndex;
            const bool isString = match("<string id=", line, index, newIndex);
            if(isString) {
                index = newIndex;
                if(index >= ls) continue;
                int id;
                const bool r = readId(line, index, newIndex, id);
                if(!r) continue;
                index = newIndex + 1;
                if(index >= ls) continue;
                if(!group) continue;
                auto& str = (*group)[id];
                while(index < ls && line[index] != '<') {
                    str.push_back(line[index++]);
                }
                continue;
            }
        }
    }
    return true;
}

bool eLanguage::loadImpl() {
    if(mLoaded) return true;
    const auto path = eGameDir::path("text.xml");
    const bool r = parse(path, mText);
    if(!r) return false;
    mLoaded = true;
    return true;
}

