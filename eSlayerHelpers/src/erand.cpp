#include "eSlayerHelpers/erand.h"

#include <assert.h>

std::random_device eRand::sDev;
std::mt19937 eRand::sRng(1/*sDev()*/);
std::uniform_int_distribution<int> eRand::sDist(0, __INT_MAX__);

int eRand::rand() {
    return sDist(sRng);
}

bool eRand::randChance(const float chance) {
    assert(chance >= 0.f);
    assert(chance <= 1.f);
    std::bernoulli_distribution dist(chance);
    return dist(sRng);
}

int eRand::rand(const int min, const int max) {
    assert(max >= min);
    std::uniform_int_distribution<int> dist(min, max);
    return dist(sRng);
}

float eRand::randF(const float min, const float max) {
    assert(max >= min);
    std::uniform_real_distribution<float> dist(min, max);
    return dist(sRng);
}

float eRand::biasedRandF(const float min, const float max,
                         const float biasStr) {
    assert(max >= min);
    const float u = randF(0.f, 1.f);
    const float biased = std::pow(u, biasStr);
    return min + (max - min) * biased;
}

uint32_t eRand::hash(uint32_t x) {
    x ^= x >> 16;
    x *= 0x7feb352d;
    x ^= x >> 15;
    x *= 0x846ca68b;
    x ^= x >> 16;
    return x;
}

float eRand::randF_seeded(const uint32_t seed,
                          const float min,
                          const float max) {
    assert(max >= min);
    const uint32_t h = hash(seed);
    const float t = (h & 0x00FFFFFF) / float(0x01000000); // [0,1)
    return min + (max - min) * t;
}
