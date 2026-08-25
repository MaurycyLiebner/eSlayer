#include "emusicvector.h"

#include <eSlayerHelpers/erand.h>

void eMusicVector::play(MIX_Mixer * const mixer,
                        MIX_Track* const track,
                        const int id,
                        const eTryPlay& tryPlay) {
    const int count = eAudioVector::count();
    if(id < 0 || id >= count) return;
    const auto& a = audio(id);

    const auto playAudio = [tryPlay, mixer, track](
            MIX_Audio * const audio) {
        const bool r = tryPlay ? tryPlay() : true;
        if(!r) return;
        MIX_SetTrackAudio(track, audio);
        MIX_PlayTrack(track, 0);
    };

    if(a) {
        playAudio(a);
    } else {
        loadAudio(mixer, id, playAudio);
    }
}

void eMusicVector::playRandomSound(MIX_Mixer * const mixer,
                                   MIX_Track* const track,
                                   const eTryPlay& tryPlay) {
    const int sc = count();
    if(sc <= 0) return;
    const int id = eRand::rand() % sc;
    play(mixer, track, id, tryPlay);
}
