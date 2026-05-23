#ifndef EBOOSTCURSETYPES_H
#define EBOOSTCURSETYPES_H

#include "eslayerhelpersexport.h"

#include "estringidmapvector.h"

class ESLAYERHELPERS_API eBoostCurseTypes {
public:
    static void load();

    static eStringIdMapVector<bool> sTypes;
private:
    static bool sLoaded;
};

#endif // EBOOSTCURSETYPES_H
