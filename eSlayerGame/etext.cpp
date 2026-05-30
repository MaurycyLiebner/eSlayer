#include "etext.h"

#include "efileloader.h"

#include <eSlayerHelpers/eexceptions.h>
#include <eSlayerHelpers/egamedir.h>

#include <cstring>
#include <fstream>
#include <sstream>
#include <filesystem>

eText eText::sInstance;

eText::eText() {}

bool eText::load() {
    return sInstance.loadImpl();
}

const std::string &eText::text(const int g, const int s) {
    return sInstance.textImpl(g, s);
}

const std::string &eText::textImpl(const int g, const int s) {
    const auto git = mText.find(g);
    if(git == mText.end()) {
        eRuntimeThrow("Groud id " + std::to_string(g) + " missing from text.xml.");
    }
    const auto& group = git->second;
    const auto sit = group.find(s);
    if(sit == group.end()) {
        eRuntimeThrow("String id " + std::to_string(s) + " missing from group " + std::to_string(g) + " text.xml.");
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

bool eText::parse(
    const std::string& path,
    eStrings& strings) {
    std::ifstream file(path);
    if(!file.good()) {
        eExceptions::logError("File missing " + path);
        return false;
    }
    const auto length { std::filesystem::file_size(path) };
    std::vector<std::byte> data(length);
    file.read(reinterpret_cast<char*>(data.data()), static_cast<long>(length));
    return parse(data, strings);
}

bool eText::parse(
    const std::vector<std::byte>& data,
    eStrings& strings) {
    std::string content(reinterpret_cast<const char*>(data.data()), data.size());
    std::istringstream file(content);

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

bool eText::loadImpl() {
    if(mLoaded) return true;
    const auto dir = "Languages";
    mText = eFileLoader::loadText(dir, "text");
    mLoaded = true;
    return true;
}
