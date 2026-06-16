#ifndef EWALLFINISHER_H
#define EWALLFINISHER_H

#include "eSlayerMapGenerator/emap.h"

#include <eSlayerHelpers/emapsettings.h>
#include <eSlayerHelpers/erect.h>

class eWallFinisher {
public:
    static void finish(
        const eRect& rect,
        const eAreaSettings& settings,
        eMap& map);
};

#endif // EWALLFINISHER_H
