#include "eservermissile.h"

int32_t eServerMissile::sNextId = 0;

eServerMissile::eServerMissile() {
    fId = sNextId++;
}
