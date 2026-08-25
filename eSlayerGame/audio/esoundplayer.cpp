#include "esoundplayer.h"

#include "esounds.h"

eSoundPlayer* eSoundPlayer::sInstance = nullptr;

eSoundPlayer::eSoundPlayer(MIX_Mixer * const mixer) :
    mMixer(mixer) {
    sInstance = this;
}

void eSoundPlayer::playButtonSound() {
    const auto id = eSounds::sSounds.id("button");
    if(id < 0) return;
    auto& b = eSounds::sSounds.get(id);
    b.playRandomSound(sInstance->mMixer);
}

MIX_Track* eSoundPlayer::requestChannel(
    MIX_Mixer * const mixer,
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
