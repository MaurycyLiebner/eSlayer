#include "eSlayerMissiles/emissilecollision.h"

#include <eSlayerHelpers/eunitdata.h>
#include <eSlayerHelpers/emissile.h>

void eMissileCollision::test(const ePointF& oldPos,
                             const ePointF& newPos,
                             const ePointF& unitPos,
                             const float collR,
                             const uint32_t charId,
                             eResult& result) {
    const float dx = newPos.fX - oldPos.fX;
    const float dy = newPos.fY - oldPos.fY;
    const float segLenSq = dx * dx + dy * dy;

    if(segLenSq < 0.0001f) {
        // Missile barely moved — point test
        const float dist = ePointF::distance(unitPos, newPos);
        if(dist <= collR && 0.f < result.fT) {
            result.fHit = true;
            result.fT = 0.f;
            result.fCharId = charId;
        }
        return;
    }

    const float fx = oldPos.fX - unitPos.fX;
    const float fy = oldPos.fY - unitPos.fY;

    const float a = segLenSq;
    const float b = 2.f * (fx * dx + fy * dy);
    const float c = fx * fx + fy * fy - collR * collR;

    // Already inside
    if(c < 0.f) {
        if(0.f < result.fT) {
            result.fHit = true;
            result.fT = 0.f;
            result.fCharId = charId;
        }
        return;
    }

    const float disc = b * b - 4.f * a * c;
    if(disc < 0.f) return;

    const float sqrtDisc = std::sqrt(disc);
    const float invDenom = 1.f / (2.f * a);

    float t1 = (-b - sqrtDisc) * invDenom;
    float t2 = (-b + sqrtDisc) * invDenom;

    float t = 2.f;

    if(t1 >= 0.f && t1 <= 1.f) t = t1;
    else if(t2 >= 0.f && t2 <= 1.f) t = t2;
    else return;

    if(t < result.fT) {
        result.fHit = true;
        result.fT = t;
        result.fCharId = charId;
    }
}

void eMissileCollision::test(const ePointF& oldPos,
                             const ePointF& newPos,
                             const eUnitData& u,
                             const eMissile& m,
                             eResult& result) {
    if(u.fHealth <= 0) return;
    if(!eTeams::areEnemies(u.fTeamId, m.fTeamId)) return;
    if(!m.fContinuousDamage) {
        if(m.fPierced.find(u.fCharId) != m.fPierced.end()) return;
    }
    const float collR = 0.5f*(u.fRadius + m.fRadius);
    return test(oldPos, newPos, u.fPos, collR, u.fCharId, result);
}
