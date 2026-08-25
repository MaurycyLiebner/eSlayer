#ifndef EMUSICPLAYER_H
#define EMUSICPLAYER_H

#include <SDL3_mixer/SDL_mixer.h>

class eMusicPlayer {
public:
    eMusicPlayer(MIX_Mixer* const mixer);

    bool initialize();

    static void incTime();
    static void playMusic(const int type);
private:
    void incTimeImpl();

    void playMusicImpl(const int type);

    void loadImpl();
    void loadMenuImpl();
    static eMusicPlayer* sInstance;

    MIX_Mixer* const mMixer;
    MIX_Track* mMusicTrack = nullptr;

    bool mLoading = false;
    int mMusicType = -1;
};

#endif // EMUSICPLAYER_H
