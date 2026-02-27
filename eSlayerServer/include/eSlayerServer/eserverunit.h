#ifndef ESERVERUNIT_H
#define ESERVERUNIT_H

#include "eslayerserverexport.h"

#include <eSlayerHelpers/epathfinderpath.h>
#include <eSlayerHelpers/evec2.h>
#include <eSlayerHelpers/eidpointf.h>

#include <queue>

struct ESLAYERSERVER_API eServerUnit {
    static int sNextCharId;
    int fCharId;
    int fTeamId;
    ePointF fPos;
    std::queue<eIdPointF> fPlanned;
};

#endif // ESERVERUNIT_H
