#ifndef ESCREENDIMENSIONS_H
#define ESCREENDIMENSIONS_H

#include "eslayerhelpersexport.h"

class ePacket;

struct ESLAYERHELPERS_API eScreenDimensions {
    int fWidth;
    int fHeight;

    void read(ePacket& p);
    void write(ePacket& p) const;
};

#endif // ESCREENDIMENSIONS_H
