#ifndef EITEMDROP_H
#define EITEMDROP_H

#include "eslayerhelpersexport.h"

#include <cstdint>

#include <nlohmann/json.hpp>
using namespace nlohmann;

struct ESLAYERHELPERS_API eItemDrop {
    uint8_t fType;
    float fChance;

    void read(const json& jitem);
};

#endif // EITEMDROP_H
