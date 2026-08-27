#ifndef ESOUNDVECTOR_H
#define ESOUNDVECTOR_H

#include "eaudiovector.h"

class eSoundVector : public eAudioVector {
public:
    using eAudioVector::eAudioVector;

    void play(MIX_Mixer * const mixer,
              const std::shared_ptr<eTrackHolder>& track,
              const int id,
              const float volume = 1.f,
              const int loop = 0);
    int playRandomSound(MIX_Mixer * const mixer,
                        const std::shared_ptr<eTrackHolder>& track,
                        const float volume = 1.f,
                        const int loop = 0);
};

#endif // ESOUNDVECTOR_H
