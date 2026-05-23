#include "eSlayerMissiles/emissileincrement.h"

#include <eSlayerHelpers/eskills.h>

eIncrementorsMap eMissileIncrement::sIncrementors;

void moveInDir(eVec2f dir, eMissile& m, const float dist) {
    dir.normalize(dist);
    if(m.fEnemy) {
        auto edir = ePointF::vector(m.fEnemyPos, m.fPos);
        const float edist = dist*std::pow(m.fTime/10.f, 1.5f);
        if(dist > edir.length()) {
            dir = edir;
        } else {
            edir.normalize(edist);
            dir = dir + edir;
            dir.normalize(dist);
        }
    }
    m.fPos = m.fPos + dir;
}

bool linear(eMissile& m, const float by) {
    m.fTime += by;
    const float dist = std::min(m.fRemDist, by*m.fSpeed);
    auto dir = ePointF::vector(m.fTo, m.fFrom);
    if(dir.length() == 0.f) return true;
    moveInDir(dir, m, dist);
    m.fRemDist -= dist;
    return m.fRemDist <= 0.f;
}

bool wave(eMissile& m, const float by) {
    const float prevTime = m.fTime;
    m.fTime += by;
    const float currTime = m.fTime;
    const float dist = std::min(m.fRemDist, by * m.fSpeed);

    auto dir = ePointF::vector(m.fTo, m.fFrom);
    if(dir.length() == 0.f) return true;
    dir.normalize(1.0f);

    const eVec2f perp(-dir.y, dir.x);

    const float amplitude = 0.5f;
    const float frequency = 0.5f;

    const float prevOffset = std::sin(prevTime * frequency) * amplitude;
    const float currOffset = std::sin(currTime * frequency) * amplitude;

    const auto forwardMove = dir * dist;
    const auto waveMove = perp * (currOffset - prevOffset);

    moveInDir(forwardMove + waveMove, m, dist);
    m.fRemDist -= dist;
    return m.fRemDist <= 0.f;
}

bool jitter(eMissile& m, const float by) {
    m.fTime += by;

    const float dist = std::min(m.fRemDist, by * m.fSpeed);
    auto dir = ePointF::vector(m.fTo, m.fFrom);
    if(dir.length() == 0.0f) return true;
    dir.normalize(1.0f);

    const eVec2f perp(-dir.y, dir.x);
    const int seed = 100*(m.fId % 100) + int(m.fTime/3.f);
    const float perpDist = dist;
    const float perpMult = eRand::randF_seeded(seed, -perpDist, perpDist);

    moveInDir(dir * dist + perp * perpMult, m, dist);
    m.fRemDist -= dist;
    return m.fRemDist <= 0.f;
}

bool spiral(eMissile& m, const float by) {
    m.fTime += by;
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

    eVec2f dir;
    dir.x = m.fFrom.fX + r * std::cos(angle) - m.fPos.fX;
    dir.y = m.fFrom.fY + r * std::sin(angle) - m.fPos.fY;

    moveInDir(dir, m, dist);
    m.fRemDist -= dist;
    return m.fRemDist <= 0.f;
}

bool staticF(eMissile& m, const float by) {
    m.fTime += by;
    const float dist = std::min(m.fRemTime, by * m.fSpeed);
    moveInDir(eVec2f{0.f, 0.f}, m, dist);
    m.fRemTime -= by;
    return m.fRemTime <= 0.f;
}

bool wandering(eMissile& m, const float by) {
    m.fTime += by;

    const float dist = std::min(m.fRemDist, by * m.fSpeed);
    const int seed = 100*(m.fId % 100) + int(m.fTime/3.f);
    const auto dir = eVec2f::random_seeded(seed);

    moveInDir(dir, m, dist);
    m.fRemDist -= dist;

    return m.fRemDist <= 0.f;
}

void eMissileIncrement::initialize() {
    sIncrementors.add("linear", &linear);
    sIncrementors.add("wave", &wave);
    sIncrementors.add("jitter", &jitter);
    sIncrementors.add("spiral", &spiral);
    sIncrementors.add("static", &staticF);
    sIncrementors.add("wandering", &wandering);

    for(const auto& it : eSkills::sSkills) {
        auto& skill = it.fValue;
        skill.fPathId = sIncrementors.id(skill.fPath);
    }
}

int eMissileIncrement::incrementorId(const std::string& name) {
    return sIncrementors.id(name);
}

bool eMissileIncrement::increment(eMissile& m, const float by) {
    const auto i = sIncrementors.get(m.fPathType);
    return i(m, by);
}
