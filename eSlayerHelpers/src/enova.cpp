#include "eSlayerHelpers/enova.h"

#include "eSlayerHelpers/epacket.h"

#include <eSlayerHelpers/eskills.h>

float radiansToDegrees(const float angle) {
    return angle * 180 / M_PI;
}

void eNova::obsticle1(
    const ePointF& pos, const float radius) {
    const auto toObstacle = ePointF::vector(pos, fCenter);
    const float dist = toObstacle.length();
    const float baseAngle = atan2(toObstacle.y, toObstacle.x);
    const float halfAngularSize = asin(radius/dist);
    const float minAngleR = baseAngle - halfAngularSize;
    const float maxAngleR = baseAngle + halfAngularSize;
    const float minAngleDeg = radiansToDegrees(minAngleR);
    const float maxAngleDeg = radiansToDegrees(maxAngleR);
    return fIntervals.subtract(minAngleDeg, maxAngleDeg);
}

void eNova::obsticle2(
    const ePointF& pos1, const ePointF& pos2) {
    const float a1 = ePointF::vector(pos1, fCenter).angle();
    const float a2 = ePointF::vector(pos2, fCenter).angle();
    return fIntervals.subtract(a1, a2);
}

bool eNova::angleInRange(const float angle) const {
    return fIntervals.angleInRange(angle);
}

void eNova::read(ePacket& p) {
    p >> fId;
    p >> fTeamId;
    p >> fMissileType;

    p >> fCenter;

    fRadius = 0.f;
    fMaxRadius = p.readFloatU8(eSkill::sRadiusMax);
    fSpeed = p.readFloatU8(eSkill::sSpeedMax);
}

void eNova::write(ePacket& p) const {
    p << fId;
    p << fTeamId;
    p << fMissileType;

    p << fCenter;

    p.writeFloatU8(fMaxRadius, eSkill::sRadiusMax);
    p.writeFloatU8(fSpeed, eSkill::sSpeedMax);
}

bool eArcIntervals::angleInRange(const float angle) const {
    for(const auto& i : *this) {
        if(i.fAngleStart > angle) continue;
        if(i.fAngleEnd < angle) continue;
        return true;
    }
    return false;
}
