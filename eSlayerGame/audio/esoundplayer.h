#ifndef ESOUNDPLAYER_H
#define ESOUNDPLAYER_H

#include <SDL3_mixer/SDL_mixer.h>

#include "eaudiovector.h"

class eSoundPlayer {
public:
    eSoundPlayer();
    ~eSoundPlayer();

    bool initialize();

    static void setVolume(const float volume);
    static void playButtonSound();
    static void playSound(const int id);
    static std::shared_ptr<eTrackHolder> playLoopSound(
        const int id, const float volume = 1.f);
private:
    void setVolumeImpl(const float volume);
    std::shared_ptr<eTrackHolder> requestTrack();
    void playSoundImpl(const int id);
    std::shared_ptr<eTrackHolder> playLoopSoundImpl(
        const int id, const float volume = 1.f);

    static eSoundPlayer* sInstance;

    MIX_Mixer* mMixer = nullptr;

    std::vector<std::shared_ptr<eTrack>> mTracks;
};

#endif // ESOUNDPLAYER_H
