#include "esoundplayer.h"

#include "esounds.h"

eSoundPlayer* eSoundPlayer::sInstance = nullptr;

eSoundPlayer::eSoundPlayer() {
    sInstance = this;
}

eSoundPlayer::~eSoundPlayer() {
    if(mMixer) MIX_DestroyMixer(mMixer);
}

bool eSoundPlayer::initialize() {
    mMixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
    if(!mMixer) {
        SDL_Log("SDL_mixer could not create mixer! %s\n",
                SDL_GetError());
        return false;
    }
    return true;
}

void eSoundPlayer::setVolume(const float volume) {
    sInstance->setVolumeImpl(volume);
}

void eSoundPlayer::playButtonSound() {
    const auto id = eSounds::sSounds.id("button");
    if(id < 0) return;
    playSound(id);
}

void eSoundPlayer::playSound(const int id, const float volume) {
    sInstance->playSoundImpl(id, volume);
}

std::shared_ptr<eTrackHolder> eSoundPlayer::playLoopSound(
    const int id, const float volume, const bool fadeIn) {
    return sInstance->playLoopSoundImpl(id, volume, fadeIn);
}

float eSoundPlayer::volumeFromDist(const float dist) {
    return std::clamp(7.f/(5.f + dist), 0.25f, 1.f);
}

void eSoundPlayer::setVolumeImpl(const float volume) {
    MIX_SetMixerGain(mMixer, volume);
}

std::shared_ptr<eTrackHolder> eSoundPlayer::requestTrack() {
    for(const auto& track : mTracks) {
        const bool r = track->used();
        if(r) continue;
        return std::make_shared<eTrackHolder>(track);
    }
    const auto track = std::make_shared<eTrack>();
    track->initialize(mMixer);
    mTracks.emplace_back(track);
    return std::make_shared<eTrackHolder>(track);
}

void eSoundPlayer::playSoundImpl(const int id, const float volume) {
    auto& b = eSounds::sSounds.get(id);
    const auto track = requestTrack();
    b.playRandomSound(mMixer, track, volume);
}

std::shared_ptr<eTrackHolder> eSoundPlayer::playLoopSoundImpl(
    const int id, const float volume, const bool fadeIn) {
    auto& b = eSounds::sSounds.get(id);
    const auto track = requestTrack();
    b.playRandomSound(mMixer, track, volume, 1, fadeIn);
    return track;
}
