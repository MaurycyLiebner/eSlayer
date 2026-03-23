#include "eSlayerMissiles/emissileincrement.h"

#include <eSlayerHelpers/eskills.h>

eIncrementorsMap eMissileIncrement::sIncrementors;

void linear(eMissile& m, const float by) {
    m.fTime += by;
    const float dist = std::min(m.fRemDistTime, by*m.fSpeed);
    auto dir = ePointF::vector(m.fTo, m.fFrom);
    if(dir.length() == 0.0f) return;
    dir.normalize();
    m.fRemDistTime -= dist;
    m.fPos = m.fPos + dir * dist;
}

void wave(eMissile& m, const float by) {
    const float prevTime = m.fTime;
    m.fTime += by;
    const float currTime = m.fTime;
    const float dist = std::min(m.fRemDistTime, by * m.fSpeed);

    auto dir = ePointF::vector(m.fTo, m.fFrom);
    if(dir.length() == 0.0f) return;
    dir.normalize(1.0f);

    const eVec2f perp(-dir.y, dir.x);

    const float amplitude = 0.5f;
    const float frequency = 0.5f;

    const float prevOffset = std::sin(prevTime * frequency) * amplitude;
    const float currOffset = std::sin(currTime * frequency) * amplitude;

    const auto forwardMove = dir * dist;
    const auto waveMove = perp * (currOffset - prevOffset);

    m.fPos = m.fPos + forwardMove + waveMove;

    m.fRemDistTime -= dist;
}

void jitter(eMissile& m, const float by) {
    m.fTime += by;

    const float dist = std::min(m.fRemDistTime, by * m.fSpeed);
    auto dir = ePointF::vector(m.fTo, m.fFrom);
    if(dir.length() == 0.0f) return;
    dir.normalize(1.0f);

    const eVec2f perp(-dir.y, dir.x);
    const int seed = 100*(m.fId % 100) + int(m.fTime/3.f);
    const float perpDist = dist;
    const float perpMult = eRand::randF(seed, -perpDist, perpDist);

    m.fPos = m.fPos + dir * dist + perp * perpMult;

    m.fRemDistTime -= dist;
}

void spiral(eMissile& m, const float by) {
    m.fTime += by;
    const float dist = std::min(m.fRemDistTime, by * m.fSpeed);

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

    m.fRemDistTime -= dist;
}

void staticF(eMissile& m, const float by) {
    m.fTime += by;
    m.fRemDistTime -= by;
}

void eMissileIncrement::initialize() {
    sIncrementors.add("linear", &linear);
    sIncrementors.add("wave", &wave);
    sIncrementors.add("jitter", &jitter);
    sIncrementors.add("spiral", &spiral);
    sIncrementors.add("static", &staticF);

    for(const auto& it : eSkills::sSkills) {
        auto& skill = it.fValue;
        skill.fPathId = sIncrementors.id(skill.fPath);
    }
}

int eMissileIncrement::incrementorId(const std::string& name) {
    return sIncrementors.id(name);
}

void eMissileIncrement::increment(eMissile& m, const float by) {
    const auto i = sIncrementors.get(m.fPathType);
    return i(m, by);
}
