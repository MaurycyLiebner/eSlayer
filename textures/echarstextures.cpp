#include "echarstextures.h"

#include "../egamedir.h"

#include <filesystem>

namespace fs = std::filesystem;

eCharsTextures eCharsTextures::sInstance;

eCharsTextures::eCharsTextures() {}

eCharTextures *eCharsTextures::get(const std::string& name) {
    return sInstance.getImpl(name);
}

void eCharsTextures::load() {
    return sInstance.loadImpl();
}

void eCharsTextures::loadImpl() {
    if(mLoaded) return;
    mLoaded = true;
    const fs::path targetDir = eGameDir::path("Textures/chars/");

    try {
        if(fs::exists(targetDir) && fs::is_directory(targetDir)) {
            for(const auto& entry : fs::directory_iterator(targetDir)) {
                if(entry.is_directory()) {
                    const auto path = entry.path();
                    const auto name = path.filename().string();
                    auto& texs = mChars[name];
                    const auto charDir = eGameDir::path("Textures/chars/" + name + "/");
                    const auto charJsonPath = charDir + name + ".json";
                    texs.load(charJsonPath);
                }
            }
        }
    } catch (const fs::filesystem_error& e) {
        printf("Filesystem error %s.\n", e.what());
    }
}

eCharTextures* eCharsTextures::getImpl(const std::string &name) {
    const auto it = mChars.find(name);
    if(it == mChars.end()) return nullptr;
    return &it->second;
}
