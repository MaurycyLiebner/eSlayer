#include "esoundplayer.h"

eSoundPlayer* eSoundPlayer::sInstance = nullptr;

eSoundPlayer::eSoundPlayer(MIX_Mixer * const mixer) :
    mMixer(mixer) {
    sInstance = this;
}

void eSoundPlayer::loadButtonSound() {
    const std::string wavsDir = "Wavs/";
    sInstance->mButton.addPath(sInstance->mMixer, wavsDir + "button.wav");
}

void eSoundPlayer::load() {
    sInstance->loadImpl();
}

bool eSoundPlayer::loaded() {
    return sInstance->mLoaded;
}

void eSoundPlayer::playButtonSound() {
    sInstance->mButton.playRandomSound(sInstance->mMixer);
}

void eSoundPlayer::loadImpl() {
    if(mLoaded) return;
    mLoaded = true;
}

MIX_Track* eSoundPlayer::requestChannel(MIX_Mixer * const mixer,
                                   const int channel) {
    const auto it = mChannels.find(channel);
    if(it == mChannels.end()) {
        const auto track = MIX_CreateTrack(mixer);
        mChannels[channel] = track;
        return track;
    } else {
        return it->second;
    }
}
