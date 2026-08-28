#ifndef EMUSICPLAYER_H
#define EMUSICPLAYER_H

#include <SDL3_mixer/SDL_mixer.h>

class eMusicPlayer {
public:
    eMusicPlayer();
    ~eMusicPlayer();

    bool initialize();

    static void setVolume(const float volume);
    static void incTime();
    static void playMusic(const int type);
private:
    void setVolumeImpl(const float volume);
    void incTimeImpl();

    void playMusicImpl(const int type);

    static eMusicPlayer* sInstance;

    MIX_Mixer* mMixer = nullptr;
    MIX_Track* mMusicTrack = nullptr;

    bool mLoading = false;
    int mMusicType = -1;
};

#endif // EMUSICPLAYER_H
