#ifndef EALIGNMENT_H
#define EALIGNMENT_H

enum class eAlignment {
    none = 0x0000,
    left = 0x0001,
    right = 0x0002,
    hcenter = 0x0004,

    top = 0x0020,
    bottom = 0x0040,
    vcenter = 0x0080,

    center = vcenter | hcenter
};

inline eAlignment operator|(const eAlignment a, const eAlignment b) {
    return static_cast<eAlignment>(static_cast<int>(a) | static_cast<int>(b));
}

inline bool operator&(const eAlignment a, const eAlignment b) {
    return static_cast<bool>(static_cast<int>(a) & static_cast<int>(b));
}

#endif // EALIGNMENT_H
