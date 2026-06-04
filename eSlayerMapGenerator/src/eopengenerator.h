#ifndef EOPENGENERATOR_H
#define EOPENGENERATOR_H

#include "../include/eSlayerMapGenerator/echamber.h"

class eOpenGenerator {
public:
    static void generate(
        const eRect& rect,
        std::vector<eChamber>& chambers,
        std::vector<eRect>& doors,
        const int margin);
};

#endif // EOPENGENERATOR_H
