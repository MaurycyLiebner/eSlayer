#include "emusicvector.h"

#include "../erand.h"

MIX_Audio* loadMusic(MIX_Mixer * const mixer,
                     const std::string& path) {
    const auto music = MIX_LoadAudio(mixer, path.c_str(), true);
    if(!music) {
        printf("Failed to load music! SDL_mixer Error: %s\n",
               SDL_GetError());
        return nullptr;
    }
    return music;
}

eMusicVector::~eMusicVector() {
    for(const auto& s : mPaths) {
        if(!s.first) continue;
        MIX_DestroyAudio(s.first);
    }
}

void eMusicVector::addPath(MIX_Mixer * const mixer,
                           const std::string& path,
                           const bool load) {
    const auto sound = load ? loadMusic(mixer, path) : nullptr;
    mPaths.push_back({sound, path});
}

void eMusicVector::play(MIX_Mixer * const mixer,
                        MIX_Track* const track,
                        const int id) {
    auto& p = mPaths[id];
    if(!p.first) p.first = loadMusic(mixer, p.second);
    if(p.first) {
        MIX_SetTrackAudio(track, p.first);
        MIX_PlayTrack(track, 0);
    }
}

void eMusicVector::playRandomSound(MIX_Mixer * const mixer,
                                   MIX_Track* const track) {
    const int sc = soundCount();
    if(sc <= 0) return;
    const int id = eRand::rand() % sc;
    play(mixer, track, id);
}
