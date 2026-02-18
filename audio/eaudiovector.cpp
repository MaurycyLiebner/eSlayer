#include "eaudiovector.h"

#include "../efileloader.h"

eAudioVector::eAudioVector() {}

eAudioVector::~eAudioVector() {
    for(const auto& s : mPaths) {
        if(!s.first) continue;
        MIX_DestroyAudio(s.first);
    }
}

MIX_Audio* eAudioVector::loadAudio(
    MIX_Mixer * const mixer,
    const std::string& path) {
    return eFileLoader::loadAudio(mixer, "Audio", path);
}

void eAudioVector::addPath(MIX_Mixer * const mixer,
                           const std::string& path,
                           const bool load) {
    const auto sound = load ? loadAudio(mixer, path) : nullptr;
    mPaths.push_back({sound, path});
}
