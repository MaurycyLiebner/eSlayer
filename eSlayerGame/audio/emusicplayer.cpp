#include "emusicplayer.h"

#include "emusic.h"

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

void eMusicPlayer::incTime() {
    sInstance->incTimeImpl();
}

void eMusicPlayer::playMusic(const int type) {
    sInstance->playMusicImpl(type);
}

void eMusicPlayer::incTimeImpl() {
    if(MIX_TrackPlaying(mMusicTrack)) return;
    playMusicImpl(mMusicType);
}

void eMusicPlayer::playMusicImpl(const int type) {
    const bool change = mMusicType != type;
    if(!change && mLoading) return;
    mMusicType = type;
    if(MIX_TrackPlaying(mMusicTrack)) {
        if(!change) return;
        MIX_StopTrack(mMusicTrack, 1000);
        return;
    }
    if(type < 0) return;
    mLoading = true;
    const auto tryPlay = [this, type]() {
        if(mMusicType != type) return false;
        mLoading = false;
        return true;
    };
    auto& m = eMusic::sMusic.get(type);
    m.playRandomSound(mMixer, mMusicTrack, tryPlay);
}