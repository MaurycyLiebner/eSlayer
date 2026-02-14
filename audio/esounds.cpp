#include "esounds.h"

#include "../egamedir.h"

eSounds* eSounds::sInstance = nullptr;

eSounds::eSounds(MIX_Mixer * const mixer) :
    mMixer(mixer) {
    sInstance = this;
}

void eSounds::loadButtonSound() {
    const std::string wavsDir{eGameDir::path("Audio/Wavs/")};
    sInstance->mButton.addPath(sInstance->mMixer, wavsDir + "button.wav");
}

void eSounds::load() {
    sInstance->loadImpl();
}

bool eSounds::loaded() {
    return sInstance->mLoaded;
}

void eSounds::playButtonSound() {
    sInstance->mButton.playRandomSound(sInstance->mMixer);
}

void eSounds::loadImpl() {
    if(mLoaded) return;
    mLoaded = true;
}

MIX_Track* eSounds::requestChannel(MIX_Mixer * const mixer,
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
