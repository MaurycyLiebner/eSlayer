#include "emusicplayer.h"

#include "emusic.h"

eMusicPlayer* eMusicPlayer::sInstance = nullptr;

eMusicPlayer::eMusicPlayer() {
    sInstance = this;
}

eMusicPlayer::~eMusicPlayer() {
    if(mMixer) MIX_DestroyMixer(mMixer);
}

bool eMusicPlayer::initialize() {
    mMixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
    if(!mMixer) {
        SDL_Log("SDL_mixer could not create mixer! %s\n",
                SDL_GetError());
        return false;
    }
    mMusicTrack = MIX_CreateTrack(mMixer);
    if(!mMusicTrack) {
        SDL_Log("Failed to create music track! SDL_mixer Error: %s\n",
                SDL_GetError());
        return false;
    }
    return true;
}

void eMusicPlayer::setVolume(const float volume) {
    sInstance->setVolumeImpl(volume);
}

void eMusicPlayer::incTime() {
    sInstance->incTimeImpl();
}

void eMusicPlayer::playMusic(const int type) {
    sInstance->playMusicImpl(type);
}

void eMusicPlayer::setVolumeImpl(const float volume) {
    MIX_SetMixerGain(mMixer, volume);
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
        MIX_StopTrack(mMusicTrack, 60000);
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