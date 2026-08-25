#include "esounds.h"

#include <eSlayerHelpers/esoundsbase.h>
#include <eSlayerHelpers/efileloaderbase.h>

eStringIdMapVector<eSoundVector> eSounds::sSounds;
bool eSounds::sLoaded = false;

void eSounds::load() {
    const auto dir = "Audio";
    for(const auto& it : eSoundsBase::sSounds) {
        const auto& name = it.fName;
        std::vector<std::string> paths;
        for(const auto& subname : it.fValue) {
            const auto path = "Wavs/" + subname + ".wav";
            const bool r = eFileLoaderBase::fileExists(dir, path);
            if(!r) {
                eRuntimeThrow("Missing \"Audio/" + path + "\" file.");
            }
            paths.emplace_back(path);
        }
        eSoundVector v(paths);
        sSounds.add(name, v);
    }
}
