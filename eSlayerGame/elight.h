#ifndef ELIGHT_H
#define ELIGHT_H

#include <eSlayerHelpers/ewalldirection.h>

struct eLight {
    eLight(const float tx,
           const float ty,
           const float radius) :
        fTX(tx), fTY(ty),
        fRadius(radius) {}

    float fTX;
    float fTY;
    float fRadius;
};

enum class eBlockerBaseType {
    object, wall
};

struct eBlockerBase {
    eBlockerBase(const eBlockerBaseType type,
                 const float tx, const float ty) :
        fType(type),
        fTX(tx), fTY(ty) {}

    eBlockerBaseType fType;
    float fTX;
    float fTY;
};

struct eObjectLightBlocker : public eBlockerBase {
    eObjectLightBlocker(const float tx,
                        const float ty,
                        const float size) :
        eBlockerBase(eBlockerBaseType::object, tx, ty),
        fSize(size) {}
    float fSize;
};

struct eWallLightBlocker : public eBlockerBase {
    eWallLightBlocker(const float tx,
                      const float ty,
                      const eWallType dir,
                      const float wallMin,
                      const float wallMax,
                      const bool minFeatherForce,
                      const bool maxFeatherForce) :
        eBlockerBase(eBlockerBaseType::wall, tx, ty),
        fDir(dir),
        fWallMin(wallMin),
        fWallMax(wallMax),
        fMinFeatherForce(minFeatherForce),
        fMaxFeatherForce(maxFeatherForce) {}

    eWallType fDir;
    float fWallMin;
    float fWallMax;
    bool fMinFeatherForce;
    bool fMaxFeatherForce;
};

#endif // ELIGHT_H
