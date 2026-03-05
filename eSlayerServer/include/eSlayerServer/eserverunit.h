#ifndef ESERVERUNIT_H
#define ESERVERUNIT_H

#include "eslayerserverexport.h"

#include <eSlayerHelpers/eunitdata.h>

struct ESLAYERSERVER_API eServerUnit : public eUnitData {
    static int sNextCharId;
};

#endif // ESERVERUNIT_H
