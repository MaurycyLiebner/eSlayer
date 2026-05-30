#ifndef EAURATYPES_H
#define EAURATYPES_H

#include "eslayerhelpersexport.h"

#include "estringidmapvector.h"

class ESLAYERHELPERS_API eAuraTypes {
public:
    static void load();

    static eStringIdMapVector<bool> sTypes;
private:
    static bool sLoaded;
};

#endif // EAURATYPES_H
