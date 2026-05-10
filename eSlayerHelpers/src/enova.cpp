#include "eSlayerHelpers/enova.h"

#include "eSlayerHelpers/epacket.h"

float normalizeAngle360(float angle) {
    if(angle >= 0.f && angle <= 360.f) return angle;
    angle = std::fmod(angle, 360.f);
    if(angle < 0.f) angle += 360.f;
    return angle;
}

float radiansToDegrees(const float angle) {
    return angle * 180 / M_PI;
}

void eNova::obsticle1(const ePointF& pos,
                      const float radius) {
    const auto toObstacle = ePointF::vector(pos, fCenter);
    const float dist = toObstacle.length();
    const float baseAngle = atan2(toObstacle.y, toObstacle.x);
    const float halfAngularSize = asin(radius/dist);
    const float minAngleR = baseAngle - halfAngularSize;
    const float maxAngleR = baseAngle + halfAngularSize;
    const float minAngleDeg = radiansToDegrees(minAngleR);
    const float maxAngleDeg = radiansToDegrees(maxAngleR);
    return subtract(minAngleDeg, maxAngleDeg);
}

void eNova::obsticle2(const ePointF& pos1,
                      const ePointF& pos2) {
    const float a1 = ePointF::vector(pos1, fCenter).angle();
    const float a2 = ePointF::vector(pos2, fCenter).angle();
    subtract(a1, a2);
}

void eNova::subtract(float minAngleDeg,
                     float maxAngleDeg) {
    const float eps = 0.01f;
    minAngleDeg = normalizeAngle360(minAngleDeg);
    maxAngleDeg = normalizeAngle360(maxAngleDeg);
    if(std::abs(maxAngleDeg - minAngleDeg) < eps) return;
    if(maxAngleDeg < minAngleDeg) {
        subtract(minAngleDeg, 360.f);
        subtract(0.f, maxAngleDeg);
        return;
    }

    for(int i = 0; i < fIntervals.size(); i++) {
        auto& curr = fIntervals[i];
        if(curr.fAngleStart + eps > maxAngleDeg) continue;
        if(curr.fAngleEnd - eps < minAngleDeg) continue;
        if(curr.fAngleStart + eps > minAngleDeg) {
            if(curr.fAngleEnd - eps < maxAngleDeg) {
                fIntervals.erase(fIntervals.begin() + i);
                i--;
            } else {
                curr.fAngleStart = maxAngleDeg;
            }
        } else if(curr.fAngleEnd - eps < maxAngleDeg) {
            curr.fAngleEnd = minAngleDeg;
        } else {
            const float endTmp = curr.fAngleEnd;
            curr.fAngleEnd = minAngleDeg;

            i++;
            fIntervals.emplace(fIntervals.begin() + i,
                               eArcInterval{maxAngleDeg, endTmp});
        }
    }
}

bool eNova::angleInRange(const float angle) const {
    for(const auto& i : fIntervals) {
        if(i.fAngleStart > angle) continue;
        if(i.fAngleEnd < angle) continue;
        return true;
    }
    return false;
}

void eNova::read(ePacket& p) {
    p >> fId;
    p >> fTeamId;
    p >> fMissileType;

    p >> fCenter;

    p >> fRadius;
    p >> fMaxRadius;
    p >> fSpeed;
}

void eNova::write(ePacket& p) const {
    p << fId;
    p << fTeamId;
    p << fMissileType;

    p << fCenter;

    p << fRadius;
    p << fMaxRadius;
    p << fSpeed;
}
