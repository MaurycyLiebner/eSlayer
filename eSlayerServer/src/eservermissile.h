#ifndef ESERVERMISSILE_H
#define ESERVERMISSILE_H

#include <eSlayerHelpers/emissile.h>

struct eServerMissile : public eMissile {
    eServerMissile();

    static int32_t sNextId;
};

#endif // ESERVERMISSILE_H
