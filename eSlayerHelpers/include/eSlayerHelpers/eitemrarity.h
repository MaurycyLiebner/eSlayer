#ifndef EITEMRARITY_H
#define EITEMRARITY_H

#include "eslayerhelpersexport.h"

#include <cstdint>
#include <string>

enum class eItemRarity : uint8_t {
    normal,
    magic,
    rare,
    set,
    unique,
    quest
};

namespace eItemRarityHelpers {
    ESLAYERHELPERS_API
    std::string name(const eItemRarity r);
    ESLAYERHELPERS_API
    eItemRarity type(const std::string& name);
}

#endif // EITEMRARITY_H
