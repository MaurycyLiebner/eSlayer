#ifndef ERAND_H
#define ERAND_H

#include "eslayerhelpersexport.h"

#include <algorithm>
#include <random>
#include <set>

class ESLAYERHELPERS_API eRand {
public:
    static int rand();
    static bool randChance(const float chance);
    static int rand(const int min,
                    const int max);
    static float randF(const float min = 0.f,
                       const float max = 1.f);
    static float biasedRandF(const float min = 0.f,
                             const float max = 1.f,
                             const float biasStr = 1.f);
    template <typename T>
    static void randomShuffle(std::vector<T>& vec);

    template <typename S>
    static auto randomElement(S& vec);

    static float randF_seeded(const uint32_t seed,
                              const float min,
                              const float max);
    static uint32_t hash(uint32_t x);
private:
    static std::random_device sDev;
    static std::mt19937 sRng;
    static std::uniform_int_distribution<int> sDist;
};

template <typename S>
inline auto eRand::randomElement(S& vec) {
    const auto id = rand(0, vec.size() - 1);
    auto it = std::begin(vec);
    std::advance(it, id);
    return *it;
}

template<typename T>
inline void eRand::randomShuffle(std::vector<T> &vec) {
    std::shuffle(vec.begin(), vec.end(), sRng);
}

#endif // ERAND_H
