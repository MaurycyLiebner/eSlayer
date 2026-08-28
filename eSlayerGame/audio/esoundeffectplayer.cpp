#include "esoundeffectplayer.h"

#include "esounds.h"

eSoundEffectPlayer* eSoundEffectPlayer::sInstance = nullptr;

eSoundEffectPlayer::eSoundEffectPlayer() {
    sInstance = this;
}

eSoundEffectPlayer::~eSoundEffectPlayer() {
    if(mMixer) MIX_DestroyMixer(mMixer);
}

bool eSoundEffectPlayer::initialize() {
    mMixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
    if(!mMixer) {
        SDL_Log("SDL_mixer could not create mixer! %s\n",
                SDL_GetError());
        return false;
    }
    const auto track = std::make_shared<eTrack>();
    track->initialize(mMixer);
    mTrack = track->fTrack;
    if(!mTrack) {
        SDL_Log("Failed to create music track! SDL_mixer Error: %s\n",
                SDL_GetError());
        return false;
    }
    mHolder = std::make_shared<eTrackHolder>(track);
    return true;
}

void eSoundEffectPlayer::setVolume(const float volume) {
    sInstance->setVolumeImpl(volume);
}

void eSoundEffectPlayer::incTime() {
    sInstance->incTimeImpl();
}

void eSoundEffectPlayer::playEffect(const int type) {
    sInstance->playEffectImpl(type);
}

void eSoundEffectPlayer::setVolumeImpl(const float volume) {
    MIX_SetMixerGain(mMixer, volume);
}

void eSoundEffectPlayer::incTimeImpl() {
    if(MIX_TrackPlaying(mTrack)) return;
    playEffectImpl(mEffectType);
}

void eSoundEffectPlayer::playEffectImpl(const int type) {
    const bool change = mEffectType != type;
    if(!change & !mStopping) {
        MIX_SetTrackLoops(mTrack, 1);
        return;
    }
    mEffectType = type;
    if(MIX_TrackPlaying(mTrack)) {
        if(!change) return;
        MIX_StopTrack(mTrack, 60000);
        mStopping = true;
        return;
    }
    mStopping = false;
    if(type < 0) return;
    auto& m = eSounds::sSounds.get(type);
    m.playRandomSound(mMixer, mHolder, 1.f, 1, true);
}