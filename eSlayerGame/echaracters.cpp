#include "echaracters.h"

#include <eSlayerHelpers/egamedir.h>

#include <filesystem>

namespace fs = std::filesystem;

eCharacters::eCharacters() {}

bool eCharacters::contains(const std::string& name) const {
    for(int i = 0; i < mCharacters.size(); i++) {
        const auto& c = mCharacters[i];
        if(c.name() == name) {
            return true;
        }
    }
    return false;
}

bool eCharacters::add(const std::string& name,
                      const bool hardcore) {
    for(int i = 0; i < mCharacters.size(); i++) {
        const auto& c = mCharacters[i];
        if(c.name() == name) {
            mCharacters.erase(mCharacters.begin() + i);
            return false;
        }
    }
    auto& c = mCharacters.emplace_back(name, hardcore);
    const bool r = c.write(eGameDir::path("Save/" + name + ".xml"),
                           eEquipment(), eAttributes(), eSkillLevels());
    return r;
}

bool eCharacters::remove(const std::string& name) {
    std::remove(eGameDir::path("Save/" + name + ".xml").c_str());
    for(int i = 0; i < mCharacters.size(); i++) {
        const auto& c = mCharacters[i];
        if(c.name() == name) {
            mCharacters.erase(mCharacters.begin() + i);
            return true;
        }
    }
    return false;
}

eCharacter eCharacters::get(const std::string& name) const {
    for(int i = 0; i < mCharacters.size(); i++) {
        const auto& c = mCharacters[i];
        if(c.name() == name) {
            return c;
        }
    }
    return eCharacter();
}

void eCharacters::load() {
    mCharacters.clear();
    const auto folder = eGameDir::path("Save/");
    std::filesystem::create_directories(folder);
    for(const auto& entry : fs::directory_iterator(folder)) {
        const bool dir = entry.is_directory();
        if(dir) continue;
        const auto path = entry.path();
        const bool isXml = path.extension() == ".xml";
        if(!isXml) continue;
        const std::string pathStr = path.u8string();
        eCharacter c;
        const bool r = c.load(pathStr, c);
        if(r) mCharacters.emplace_back(c);
    }
}
