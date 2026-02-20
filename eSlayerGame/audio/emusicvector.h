#ifndef EMUSICVECTOR_H
#define EMUSICVECTOR_H

#include "eaudiovector.h"

class eMusicVector : public eAudioVector {
public:
    void play(MIX_Mixer * const mixer,
              MIX_Track * const track,
              const int id);
    void playRandomSound(MIX_Mixer * const mixer,
                         MIX_Track * const track);
};

#endif // EMUSICVECTOR_H
