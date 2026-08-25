#ifndef EMUSICPLAYER_H
#define EMUSICPLAYER_H

#include "emusicvector.h"

enum class eMusicType {
    none, intro
};

class eMusicPlayer {
public:
    eMusicPlayer(MIX_Mixer* const mixer);

    bool initialize();

    static void loadMenu();
    static void load();
    static bool loaded();

    static void incTime();

    static void playMenuMusic();
private:
    void incTimeImpl();

    void playMenuMusicImpl();

    void loadImpl();
    void loadMenuImpl();
    static eMusicPlayer* sInstance;

    MIX_Mixer* const mMixer;
    MIX_Track* mMusicTrack = nullptr;

    bool mLoaded{false};
    bool mMenuLoaded{false};
    eMusicType mMusicType{eMusicType::none};

    eMusicVector mIntroMusic;
};

#endif // EMUSICPLAYER_H
