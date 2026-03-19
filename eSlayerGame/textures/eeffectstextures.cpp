#include "eeffectstextures.h"

#include "../efileloader.h"

#include <eSlayerHelpers/eexceptions.h>

bool eEffectsTextures::sLoaded = false;
eStringIdMapVector<eEffectTextures>
eEffectsTextures::sEffects;

void eEffectsTextures::load() {
    if(sLoaded) return;
    sLoaded = true;

    const auto dir = "Textures";

    std::vector<std::string> effs;
    try {
        const auto jdata = eFileLoader::parse(dir, "effects/effects.json");
        effs = jdata.get<std::vector<std::string>>();
    } catch(...) {
        eRuntimeThrow("Failed to parse " + dir + "/effects/effects.json");
    }

    for(const auto& name : effs) {
        try {
            const int oldId = sEffects.id(name);
            if(oldId != -1) {
                eExceptions::showDialog("Duplicate effect '" + name + "' in " + dir + "/effects/effects.json");
                continue;
            }
            eEffectTextures texs;
            texs.setName(name);
            sEffects.add(name, texs);
        } catch(const std::exception& e) {
            eExceptions::showDialog(e);
        }
    }
}
