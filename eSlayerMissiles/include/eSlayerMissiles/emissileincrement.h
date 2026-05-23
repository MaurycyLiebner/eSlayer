#ifndef EMISSILEINCREMENT_H
#define EMISSILEINCREMENT_H

#include "eslayermissilesexport.h"

#include <eSlayerHelpers/emissile.h>
#include <eSlayerHelpers/estringidmapvector.h>

using eIncrementorsMap = eStringIdMapVector<bool (*)(eMissile& m, const float by)>;

class ESLAYERMISSILES_API eMissileIncrement {
public:
    static void initialize();

    static int incrementorId(const std::string& name);
    static bool increment(eMissile& m, const float by);
private:
    static eIncrementorsMap sIncrementors;
};

#endif // EMISSILEINCREMENT_H
