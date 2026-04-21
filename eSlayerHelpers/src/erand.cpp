#include "../include/eSlayerHelpers/erand.h"

std::random_device eRand::sDev;
std::mt19937 eRand::sRng(sDev());
std::uniform_int_distribution<int> eRand::sDist(0, __INT_MAX__);

int eRand::rand() {
    return sDist(sRng);
}

bool eRand::randChance(const float chance) {
    std::bernoulli_distribution dist(chance);
    return dist(sRng);
}

int eRand::rand(const int min, const int max) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(sRng);
}

float eRand::randF(const float min, const float max) {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(sRng);
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
    const uint32_t h = hash(seed);
    const float t = (h & 0x00FFFFFF) / float(0x01000000); // [0,1)
    return min + (max - min) * t;
}
