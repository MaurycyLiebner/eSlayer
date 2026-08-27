#include "esoundvector.h"

#include <eSlayerHelpers/erand.h>

void eSoundVector::play(MIX_Mixer * const mixer,
                        const std::shared_ptr<eTrackHolder>& track,
                        const int id,
                        const float volume,
                        const int loop) {
    const int count = eAudioVector::count();
    if(id < 0 || id >= count) return;
    const auto& a = audio(id);
    const auto playAudio = [mixer, track, loop, volume](
            MIX_Audio * const audio) {
        const auto mtrack = track->fTrack->fTrack;
        MIX_SetTrackAudio(mtrack, audio);
        MIX_SetTrackGain(mtrack, volume);
        MIX_SetTrackLoops(mtrack, loop);
        MIX_PlayTrack(mtrack, 0);
    };

    if(a) {
        playAudio(a);
    } else {
        loadAudio(mixer, id, playAudio);
    }
}

int eSoundVector::playRandomSound(MIX_Mixer * const mixer,
                                  const std::shared_ptr<eTrackHolder>& track,
                                  const float volume,
                                  const int loop) {
    const int sc = count();
    if(sc <= 0) return -1;
    const int id = eRand::rand() % sc;
    play(mixer, track, id, volume, loop);
    return id;
}
