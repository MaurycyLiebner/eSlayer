#include "eSlayerHelpers/emissilecollision.h"

void eMissileCollision::test(const ePointF& oldPos,
                             const ePointF& newPos,
                             const ePointF& unitPos,
                             const float collR,
                             const int charId,
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

    // Swept segment-vs-circle test
    // Solve: |oldPos + t*d - C|² = collR²
    const float fx = oldPos.fX - unitPos.fX;
    const float fy = oldPos.fY - unitPos.fY;
    const float a = segLenSq;
    const float b = 2.f * (fx * dx + fy * dy);
    const float c = fx * fx + fy * fy - collR * collR;
    const float disc = b * b - 4.f * a * c;
    if(disc < 0.f) return;
    float t = (-b - std::sqrt(disc)) / (2.f * a);
    // If entry point is behind us, check if
    // we're already inside the circle at t=0
    if(t < 0.f) t = 0.f;
    if(t <= 1.f && t < result.fT) {
        result.fHit = true;
        result.fT = t;
        result.fCharId = charId;
    }
}
