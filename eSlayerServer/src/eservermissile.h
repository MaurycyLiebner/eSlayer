#ifndef ESERVERMISSILE_H
#define ESERVERMISSILE_H

#include <eSlayerHelpers/emissile.h>

#include <functional>

class eServerUnit;

struct eServerMissile : public eMissile {
    eServerMissile();

    static int32_t sNextId;

    using eHitAction = std::function<void(eServerUnit&)>;
    eHitAction fHitAction;
};

#endif // ESERVERMISSILE_H
