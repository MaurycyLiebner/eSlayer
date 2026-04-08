#ifndef ECLASSES_H
#define ECLASSES_H

#include "eclass.h"

#include "estringidmapvector.h"

class ESLAYERHELPERS_API eClasses {
public:
    static void load();

    static eStringIdMapVector<eClass> sClasses;
private:
    static bool sLoaded;
};

#endif // ECLASSES_H
