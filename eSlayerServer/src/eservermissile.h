#ifndef ESERVERMISSILE_H
#define ESERVERMISSILE_H

#include <eSlayerHelpers/emissile.h>

#include <functional>

class eServerUnit;

struct eServerMissile : public eMissile {
    eServerMissile();

    static uint32_t sNextId;

    using eHitAction = std::function<void(
        eServerUnit&, const eServerMissile&)>;
    eHitAction fHitAction;

    int fConsecutive = 0;
    float fTotalTime = 0.f;
    float fTotalDist = 0.f;
};

#endif // ESERVERMISSILE_H
