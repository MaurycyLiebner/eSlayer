#ifndef ELIGHT_H
#define ELIGHT_H

#include <eSlayerHelpers/ewalldirection.h>
#include <eSlayerHelpers/epoint.h>

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
    object, rect, wall
};

struct eBlockerBase {
    eBlockerBase(const eBlockerBaseType type,
                 const float tx, const float ty,
                 const float w, const float h) :
        fType(type),
        fTX(tx), fTY(ty),
        fWidth(w), fHeight(h) {}

    eBlockerBaseType fType;
    float fTX;
    float fTY;

    float fWidth;
    float fHeight;
};

struct eObjectLightBlocker : public eBlockerBase {
    eObjectLightBlocker(const float tx,
                        const float ty,
                        const float size) :
        eBlockerBase(eBlockerBaseType::object,
                     tx, ty, size, size),
        fSize(size) {}
    float fSize;
};

struct eRectLightBlocker : public eBlockerBase {
    eRectLightBlocker(const float tx,
                      const float ty,
                      const float width,
                      const float height) :
        eBlockerBase(eBlockerBaseType::rect,
                     tx, ty, width, height) {}
};

struct eWallLightBlocker : public eBlockerBase {
    eWallLightBlocker(const float tx,
                      const float ty,
                      const eWallType dir,
                      const float wallMin,
                      const float wallMax,
                      const bool minFeatherForce,
                      const bool maxFeatherForce) :
        eBlockerBase(eBlockerBaseType::wall,
                     tx, ty, 1.f, 1.f),
        fDir(dir) {
        fITX = std::round(tx);
        fITY = std::round(ty);
        fMinFeather = minFeatherForce || wallMin != 0.f;
        fMaxFeather = maxFeatherForce || wallMax != 1.f;
        switch(dir) {
        case eWallType::topLeft:
            fP1.fX = fTX;
            fP1.fY = fTY + wallMin;
            fP2.fX = fTX;
            fP2.fY = fTY + wallMax;
            break;
        case eWallType::topRight:
            fP1.fX = fTX + wallMin;
            fP1.fY = fTY;
            fP2.fX = fTX + wallMax;
            fP2.fY = fTY;
            break;
        }
    }

    eWallType fDir;

    int fITX;
    int fITY;
    bool fMinFeather;
    bool fMaxFeather;
    ePointF fP1;
    ePointF fP2;
};

#endif // ELIGHT_H
