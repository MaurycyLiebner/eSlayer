#ifndef EEFFECTSETTINGS_H
#define EEFFECTSETTINGS_H

enum class eEffectType {
    distort,
    rain,
    space
};

struct eEffectSettings {
    eEffectType fType;
    float fSpeed;
    float fScale;
};

#endif // EEFFECTSETTINGS_H
