#ifndef EDUNGEONGENERATOR_H
#define EDUNGEONGENERATOR_H

#include "echamber.h"

class eDungeonGenerator {
public:
    static void generate(const eRect& rect,
                         std::vector<eChamber>& chambers,
                         std::vector<eRect>& doors);
};

#endif // EDUNGEONGENERATOR_H
