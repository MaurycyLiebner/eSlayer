#include "esoundvector.h"

#include "../erand.h"
#include <filesystem>

MIX_Audio* loadSound(MIX_Mixer * const mixer,
                     const std::string& path) {
    const auto wav = MIX_LoadAudio(mixer, path.c_str(), false);
    if(!wav) {
        printf("Failed to load sound '%s'!\n SDL_mixer Error: %s\n",
               path.c_str(), SDL_GetError());
        return nullptr;
    }
    return wav;
}

eSoundVector::~eSoundVector() {
    for(const auto& s : mPaths) {
        if(!s.first) continue;
        MIX_DestroyAudio(s.first);
    }
}

void eSoundVector::addPath(MIX_Mixer * const mixer,
                           const std::string& path,
                           const bool load) {
    const bool e = std::filesystem::exists(path);
    if(!e) printf("Missing audio file %s\n", path.c_str());
    const auto sound = load ? loadSound(mixer, path) : nullptr;
    mPaths.push_back({sound, path});
}

void eSoundVector::play(MIX_Mixer * const mixer,
                        const int id,
                        MIX_Track* const channel) {
    const int idMax = mPaths.size();
    if(id < 0 || id >= idMax) return;
    auto& p = mPaths[id];
    if(!p.first) p.first = loadSound(mixer, p.second);
    if(p.first) {
        if(channel) {
            MIX_SetTrackAudio(channel, p.first);
            MIX_PlayTrack(channel, 0);
        } else {
            MIX_PlayAudio(mixer, p.first);
        }
    }
}

void eSoundVector::playRandomSound(MIX_Mixer * const mixer,
                                   MIX_Track * const channel) {
    const int sc = soundCount();
    if(sc <= 0) return;
    const int id = eRand::rand() % sc;
    play(mixer, id, channel);
}
