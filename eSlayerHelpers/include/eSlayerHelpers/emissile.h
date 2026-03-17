#ifndef EMISSILE_H
#define EMISSILE_H

#include "eslayerhelpersexport.h"

#include "epositioned.h"

class ePacket;

struct ESLAYERHELPERS_API eMissile : public ePositioned {
    uint8_t fType;
    std::vector<ePointF> fPath;

    void increment(const float by);

    void read(ePacket& p);
    void write(ePacket& p) const;
};

#endif // EMISSILE_H
