#include "eSlayerHelpers/enova.h"

float normalizeAngle360(float angle) {
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

void eNova::obsticle4(const ePointF& pos1,
                      const ePointF& pos2,
                      const ePointF& pos3,
                      const ePointF& pos4) {
    const float a0 = ePointF::vector(pos1, fCenter).angle();
    const float a1 = ePointF::vector(pos2, fCenter).angle();
    const float a2 = ePointF::vector(pos3, fCenter).angle();
    const float a3 = ePointF::vector(pos4, fCenter).angle();
    const float minAngle = std::min(a0, std::min(a1, std::min(a2, a3)));
    const float maxAngle = std::max(a0, std::max(a1, std::max(a2, a3)));
    return subtract(minAngle, maxAngle);
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
