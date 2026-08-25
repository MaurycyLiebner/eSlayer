#ifndef ESOUNDPLAYER_H
#define ESOUNDPLAYER_H

#include <SDL3_mixer/SDL_mixer.h>

#include <map>

class eSoundPlayer {
public:
    eSoundPlayer(MIX_Mixer* const mixer);

    static void playButtonSound();
    static void playSound(const int id);
private:
    MIX_Track* requestChannel(MIX_Mixer * const mixer,
                              const int channel);

    static eSoundPlayer* sInstance;

    MIX_Mixer* const mMixer;

    std::map<int, MIX_Track*> mChannels;
};

#endif // ESOUNDPLAYER_H
