#include "eservermissile.h"

uint32_t eServerMissile::sNextId = 1;

eServerMissile::eServerMissile() {
    fId = sNextId++;
}
