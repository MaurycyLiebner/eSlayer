#ifndef ESERVERUNIT_H
#define ESERVERUNIT_H

#include "eslayerserverexport.h"

#include <eSlayerHelpers/epathfinderpath.h>
#include <eSlayerHelpers/evec2.h>

struct ESLAYERSERVER_API eServerUnit {
    static int sNextCharId;
    int fCharId;
    ePointF fPos;
    eVec2d fDir;
    ePathFinderPath fPath;
};

#endif // ESERVERUNIT_H
