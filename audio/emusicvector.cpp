#include "emusicvector.h"

#include "../erand.h"

void eMusicVector::play(MIX_Mixer * const mixer,
                        MIX_Track* const track,
                        const int id) {
    auto& p = mPaths[id];
    if(!p.first) p.first = loadAudio(mixer, p.second);
    if(p.first) {
        MIX_SetTrackAudio(track, p.first);
        MIX_PlayTrack(track, 0);
    }
}

void eMusicVector::playRandomSound(MIX_Mixer * const mixer,
                                   MIX_Track* const track) {
    const int sc = soundCount();
    if(sc <= 0) return;
    const int id = eRand::rand() % sc;
    play(mixer, track, id);
}
