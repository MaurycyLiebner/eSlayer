#ifndef ESOUNDVECTOR_H
#define ESOUNDVECTOR_H

#include "eaudiovector.h"

class eSoundVector : public eAudioVector {
public:
    using eAudioVector::eAudioVector;

    void play(MIX_Mixer * const mixer,
              const int id,
              MIX_Track * const channel = nullptr);
    void playRandomSound(MIX_Mixer * const mixer,
                         MIX_Track * const channel = nullptr);
};

#endif // ESOUNDVECTOR_H
