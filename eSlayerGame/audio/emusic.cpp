#include "emusic.h"

#include <eSlayerHelpers/emusicbase.h>
#include <eSlayerHelpers/efileloaderbase.h>

eStringIdMapVector<eMusicVector> eMusic::sMusic;
bool eMusic::sLoaded = false;

void eMusic::load() {
    const auto dir = "Audio";
    for(const auto& it : eMusicBase::sMusic) {
        const auto& name = it.fName;
        std::vector<std::string> paths;
        for(const auto& subname : it.fValue) {
            const auto path = "Music/" + subname + ".mp3";
            const bool r = eFileLoaderBase::fileExists(dir, path);
            if(!r) {
                eRuntimeThrow("Missing \"Audio/" + path + "\" file.");
            }
            paths.emplace_back(path);
        }
        eMusicVector v(paths);
        sMusic.add(name, v);
    }
}
