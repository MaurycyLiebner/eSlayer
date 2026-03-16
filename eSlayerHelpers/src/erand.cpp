#include "../include/eSlayerHelpers/erand.h"

#if (defined (_WIN32) || defined (_WIN64))
    #define __INT_MAX__ INT_MAX
#endif

std::random_device eRand::sDev;
std::mt19937 eRand::sRng(sDev());
std::uniform_int_distribution<int> eRand::sDist(0, __INT_MAX__);

int eRand::rand() {
    return sDist(sRng);
}

float eRand::randF(const float min, const float max) {
    const int margin = 100000;
    return min + (max - min)*(rand() % (margin + 1))/margin;
}
