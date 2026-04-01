#ifndef EGROUNDITEM_H
#define EGROUNDITEM_H

#include "eslayerhelpersexport.h"

#include "epositioned.h"

#include "eitemdata.h"

#include "eitemrarity.h"

class ePacket;

struct ESLAYERHELPERS_API eGroundItem : public ePositioned {
    uint32_t fItemId;
    uint8_t fDataId = 0;
    eItemType fType = eItemType::none;
    uint8_t fSubType = 0;

    eItemRarity fRarity = eItemRarity::normal;
    uint8_t fSockets = 0;

    void read(ePacket& p);
    void write(ePacket& p) const;
};

#endif // EGROUNDITEM_H
