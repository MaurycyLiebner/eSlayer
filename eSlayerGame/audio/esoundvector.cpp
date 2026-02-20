#include "esoundvector.h"

#include <eSlayerHelpers/erand.h>

void eSoundVector::play(MIX_Mixer * const mixer,
                        const int id,
                        MIX_Track* const channel) {
    const int idMax = mPaths.size();
    if(id < 0 || id >= idMax) return;
    auto& p = mPaths[id];
    if(!p.first) p.first = loadAudio(mixer, p.second);
    if(p.first) {
        if(channel) {
            MIX_SetTrackAudio(channel, p.first);
            MIX_PlayTrack(channel, 0);
        } else {
            MIX_PlayAudio(mixer, p.first);
        }
    }
}

void eSoundVector::playRandomSound(MIX_Mixer * const mixer,
                                   MIX_Track * const channel) {
    const int sc = soundCount();
    if(sc <= 0) return;
    const int id = eRand::rand() % sc;
    play(mixer, id, channel);
}
