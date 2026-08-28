#ifndef ESOUNDEFFECTPLAYER_H
#define ESOUNDEFFECTPLAYER_H

#include "eaudiovector.h"

#include <SDL3_mixer/SDL_mixer.h>

class eSoundEffectPlayer {
public:
    eSoundEffectPlayer();
    ~eSoundEffectPlayer();

    bool initialize();

    static void setVolume(const float volume);
    static void incTime();
    static void playEffect(const int type);
private:
    void setVolumeImpl(const float volume);
    void incTimeImpl();

    void playEffectImpl(const int type);

    static eSoundEffectPlayer* sInstance;

    MIX_Mixer* mMixer = nullptr;
    std::shared_ptr<eTrackHolder> mHolder;
    MIX_Track* mTrack = nullptr;

    bool mStopping = false;
    int mEffectType = -1;
};

#endif // ESOUNDEFFECTPLAYER_H
