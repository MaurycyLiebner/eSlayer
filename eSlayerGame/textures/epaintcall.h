#ifndef EPAINTCALL_H
#define EPAINTCALL_H

#include "etexture.h"

struct ePaintCall {
    float fX;
    float fY;
    std::shared_ptr<eTexture> fTex;
};

#endif // EPAINTCALL_H
