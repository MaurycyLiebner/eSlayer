#include "eSlayerMissiles/emissileincrement.h"

eIncrementorsMap eMissileIncrement::sIncrementors;

void linear(eMissile& m, const float by) {
    const float dist = std::min(m.fRemDist, by*m.fSpeed);
    eVec2f dir = ePointF::vector(m.fTo, m.fFrom);
    dir.normalize(dist);
    m.fRemDist -= dist;
    m.fPos = m.fPos + dir;
}

void spiral(eMissile& m, const float by) {
    const float dist = std::min(m.fRemDist, by * m.fSpeed);

    const eVec2f v = ePointF::vector(m.fPos, m.fFrom);

    float r = v.length();
    float angle = std::atan2(v.y, v.x);

    const float angularSpeed = 2.0f;
    const float radialSpeed  = 0.5f;

    const float arc = std::sqrt((r * angularSpeed) * (r * angularSpeed) +
                                radialSpeed * radialSpeed);

    float dTheta = 0.0f;
    float dRadius = 0.0f;

    if(arc > 0.0f) {
        const float scale = dist / arc;
        dTheta  = angularSpeed * scale;
        dRadius = radialSpeed  * scale;
    }

    angle += dTheta;
    r += dRadius;

    m.fPos.fX = m.fFrom.fX + r * std::cos(angle);
    m.fPos.fY = m.fFrom.fY + r * std::sin(angle);

    m.fRemDist -= dist;
}

void eMissileIncrement::initialize() {
    sIncrementors.add("linear", &linear);
    sIncrementors.add("spiral", &spiral);
}

int eMissileIncrement::incrementorId(const std::string& name) {
    return sIncrementors.id(name);
}

void eMissileIncrement::increment(eMissile& m, const float by) {
    const auto i = sIncrementors.get(m.fPathType);
    return i(m, by);
}
