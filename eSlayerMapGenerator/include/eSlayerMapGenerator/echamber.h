#ifndef ECHAMBER_H
#define ECHAMBER_H

#include "eslayermapgeneratorexport.h"

#include <eSlayerHelpers/erect.h>

struct ESLAYERMAPGENERATOR_API eChamber {
    eChamber();
    eChamber(const eRect& r);
    eChamber(const std::vector<eRect>& r);

    std::vector<eRect> fRects;

    int area() const;
    bool contains(const ePoint& p) const;
    bool wallTL(const ePoint& p) const;
    bool wallTR(const ePoint& p) const;
};

#endif // ECHAMBER_H
