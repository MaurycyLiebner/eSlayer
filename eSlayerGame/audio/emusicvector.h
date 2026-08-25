#ifndef EMUSICVECTOR_H
#define EMUSICVECTOR_H

#include "eaudiovector.h"

using eTryPlay = std::function<bool()>;

class eMusicVector : public eAudioVector {
public:
    using eAudioVector::eAudioVector;

    void play(MIX_Mixer * const mixer,
              MIX_Track * const track,
              const int id,
              const eTryPlay& tryPlay);
    void playRandomSound(MIX_Mixer * const mixer,
                         MIX_Track * const track,
                         const eTryPlay& tryPlay);
};

#endif // EMUSICVECTOR_H
