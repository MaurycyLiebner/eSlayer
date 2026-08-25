#include "esoundvector.h"

#include <eSlayerHelpers/erand.h>

void eSoundVector::play(MIX_Mixer * const mixer,
                        const int id,
                        MIX_Track * const channel) {
    const int count = eAudioVector::count();
    if(id < 0 || id >= count) return;
    const auto& a = audio(id);

    const auto playAudio = [mixer, channel](MIX_Audio * const audio) {
        if(channel) {
            MIX_SetTrackAudio(channel, audio);
            MIX_PlayTrack(channel, 0);
        } else {
            MIX_PlayAudio(mixer, audio);
        }
    };

    if(a) {
        playAudio(a);
    } else {
        loadAudio(mixer, id, playAudio);
    }
}

void eSoundVector::playRandomSound(MIX_Mixer * const mixer,
                                   MIX_Track * const channel) {
    const int sc = count();
    if(sc <= 0) return;
    const int id = eRand::rand() % sc;
    play(mixer, id, channel);
}
