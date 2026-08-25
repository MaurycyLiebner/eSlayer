#ifndef ESOUNDPLAYER_H
#define ESOUNDPLAYER_H

#include "esoundvector.h"

#include <map>

class eSoundPlayer {
public:
    eSoundPlayer(MIX_Mixer* const mixer);

    static void loadButtonSound();

    static void load();
    static bool loaded();

    static void playButtonSound();
private:
    void loadImpl();

    MIX_Track* requestChannel(MIX_Mixer * const mixer,
                              const int channel);

    static eSoundPlayer* sInstance;

    MIX_Mixer* const mMixer;

    bool mLoaded{false};

    std::map<int, MIX_Track*> mChannels;

    eSoundVector mButton;
};

#endif // ESOUNDPLAYER_H
