#include "emusic.h"

eMusic* eMusic::sInstance = nullptr;

eMusic::eMusic(MIX_Mixer * const mixer) :
    mMixer(mixer) {
    sInstance = this;
}

bool eMusic::initialize() {
    mMusicTrack = MIX_CreateTrack(mMixer);
    if(!mMusicTrack) {
        SDL_Log("Failed to create music track! SDL_mixer Error: %s\n",
                SDL_GetError());
    }
    return mMusicTrack;
}

void eMusic::loadMenu() {
    sInstance->loadMenuImpl();
}

void eMusic::load() {
    sInstance->loadImpl();
}

bool eMusic::loaded() {
    return sInstance->mLoaded;
}

void eMusic::incTime() {
    sInstance->incTimeImpl();
}

void eMusic::playMenuMusic() {
    sInstance->playMenuMusicImpl();
}

void eMusic::incTimeImpl() {
    if(MIX_TrackPlaying(mMusicTrack)) return;
    switch(mMusicType) {
    case eMusicType::none:
        break;
    case eMusicType::intro:
        playMenuMusicImpl();
        break;
    }
}

void eMusic::playMenuMusicImpl() {
    const bool change = mMusicType != eMusicType::intro;
    mMusicType = eMusicType::intro;
    if(MIX_TrackPlaying(mMusicTrack)) {
        if(!change) return;
        MIX_StopTrack(mMusicTrack, 1000);
        return;
    }
    mIntroMusic.playRandomSound(mMixer, mMusicTrack);
}

void eMusic::loadImpl() {
    if(mLoaded) return;
    mLoaded = true;
}

void eMusic::loadMenuImpl() {
    if(mMenuLoaded) return;
    mMenuLoaded = true;
    const std::string dir = "Music/";
    mIntroMusic.addPath(mMixer, dir + "intro.mp3");
}
