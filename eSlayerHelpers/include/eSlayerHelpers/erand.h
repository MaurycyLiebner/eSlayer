#ifndef ERAND_H
#define ERAND_H

#include "eslayerhelpersexport.h"

#include <algorithm>
#include <random>

class ESLAYERHELPERS_API eRand {
public:
    static int rand();
    template <typename T>
    static void randomShuffle(std::vector<T>& vec);
private:
    static std::random_device sDev;
    static std::mt19937 sRng;
    static std::uniform_int_distribution<int> sDist;
};

template<typename T>
inline void eRand::randomShuffle(std::vector<T> &vec) {
    std::shuffle(vec.begin(), vec.end(), sDev);
}

#endif // ERAND_H
