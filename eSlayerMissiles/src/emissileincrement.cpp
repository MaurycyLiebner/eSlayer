#include "eSlayerMissiles/emissileincrement.h"

eIncrementorsMap eMissileIncrement::sIncrementors;

void linear(eMissile& m, const float by) {
    const float dist = std::min(m.fRemDist, by*m.fSpeed);
    eVec2f dir = ePointF::vector(m.fTo, m.fFrom);
    dir.normalize(dist);
    m.fRemDist -= dist;
    m.fPos = m.fPos + dir;
}

void eMissileIncrement::initialize() {
    sIncrementors.add("linear", &linear);
}

int eMissileIncrement::incrementorId(const std::string& name) {
    return sIncrementors.id(name);
}

void eMissileIncrement::increment(eMissile& m, const float by) {
    const auto i = sIncrementors.get(m.fPathType);
    return i(m, by);
}
