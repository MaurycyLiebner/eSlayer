#include "emusicplayer.h"

eMusicPlayer* eMusicPlayer::sInstance = nullptr;

eMusicPlayer::eMusicPlayer(MIX_Mixer * const mixer) :
    mMixer(mixer) {
    sInstance = this;
}

bool eMusicPlayer::initialize() {
    mMusicTrack = MIX_CreateTrack(mMixer);
    if(!mMusicTrack) {
        SDL_Log("Failed to create music track! SDL_mixer Error: %s\n",
                SDL_GetError());
    }
    return mMusicTrack;
}

void eMusicPlayer::loadMenu() {
    sInstance->loadMenuImpl();
}

void eMusicPlayer::load() {
    sInstance->loadImpl();
}

bool eMusicPlayer::loaded() {
    return sInstance->mLoaded;
}

void eMusicPlayer::incTime() {
    sInstance->incTimeImpl();
}

void eMusicPlayer::playMenuMusic() {
    sInstance->playMenuMusicImpl();
}

void eMusicPlayer::incTimeImpl() {
    if(MIX_TrackPlaying(mMusicTrack)) return;
    switch(mMusicType) {
    case eMusicType::none:
        break;
    case eMusicType::intro:
        playMenuMusicImpl();
        break;
    }
}

void eMusicPlayer::playMenuMusicImpl() {
    const bool change = mMusicType != eMusicType::intro;
    mMusicType = eMusicType::intro;
    if(MIX_TrackPlaying(mMusicTrack)) {
        if(!change) return;
        MIX_StopTrack(mMusicTrack, 1000);
        return;
    }
    mIntroMusic.playRandomSound(mMixer, mMusicTrack);
}

void eMusicPlayer::loadImpl() {
    if(mLoaded) return;
    mLoaded = true;
}

void eMusicPlayer::loadMenuImpl() {
    if(mMenuLoaded) return;
    mMenuLoaded = true;
    const std::string dir = "Music/";
    mIntroMusic.addPath(mMixer, dir + "intro.mp3");
}
