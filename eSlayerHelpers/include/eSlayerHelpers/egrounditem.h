#ifndef EGROUNDITEM_H
#define EGROUNDITEM_H

#include "eslayerhelpersexport.h"

#include "epositioned.h"
#include "eitem.h"

class ePacket;

struct ESLAYERHELPERS_API eGroundItem :
    public ePositioned,
    public eItemBase {};

#endif // EGROUNDITEM_H
