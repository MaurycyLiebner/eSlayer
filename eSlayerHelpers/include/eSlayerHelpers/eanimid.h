#ifndef EANIMID_H
#define EANIMID_H

#include "eslayerhelpersexport.h"

#include <cstdint>

struct ESLAYERHELPERS_API eAnimId {
    uint8_t fValue = 255;

    void increment(const uint8_t by) {
        fValue += by;
    }

    static bool isAfter(const eAnimId& a, const eAnimId& b) {
        return isAfter(a.fValue, b.fValue);
    }

    static bool isAfter(const uint8_t a, const uint8_t b) {
        return static_cast<int8_t>(a - b) > 0;
    }
};

#endif // EANIMID_H
