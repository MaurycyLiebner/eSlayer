#ifndef EWEAPONCLASS_H
#define EWEAPONCLASS_H

#include "eweapontype.h"
#include "eitemdata.h"
#include "estringidmapvector.h"

struct eWeaponClass {
    eWeaponType fType;
    bool fTwoHanded;
    std::vector<eItemType> fSecondHand;
};

class ESLAYERHELPERS_API eWeaponClasses {
public:
    static eStringIdMapVector<eWeaponClass> sClasses;

    static void load();
};

#endif // EWEAPONCLASS_H
