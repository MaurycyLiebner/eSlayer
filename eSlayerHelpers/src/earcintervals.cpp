#include "eSlayerHelpers/earcintervals.h"

#include <cmath>

float normalizeAngle360(float angle) {
    if(angle >= 0.f && angle <= 360.f) return angle;
    angle = std::fmod(angle, 360.f);
    if(angle < 0.f) angle += 360.f;
    return angle;
}

void eArcIntervals::subtract(
    float minAngleDeg, float maxAngleDeg) {
    const float eps = 0.01f;
    minAngleDeg = normalizeAngle360(minAngleDeg);
    maxAngleDeg = normalizeAngle360(maxAngleDeg);
    if(std::abs(maxAngleDeg - minAngleDeg) < eps) return;
    if(maxAngleDeg < minAngleDeg) {
        if(minAngleDeg - maxAngleDeg < 180.f) {
            return subtract(maxAngleDeg, minAngleDeg);
        }
        subtract(minAngleDeg, 360.f);
        subtract(0.f, maxAngleDeg);
        return;
    }

    for(int i = 0; i < size(); i++) {
        auto& curr = (*this)[i];
        if(curr.fAngleStart + eps > maxAngleDeg) continue;
        if(curr.fAngleEnd - eps < minAngleDeg) continue;
        if(curr.fAngleStart + eps > minAngleDeg) {
            if(curr.fAngleEnd - eps < maxAngleDeg) {
                erase(begin() + i);
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
            emplace(begin() + i, maxAngleDeg, endTmp);
        }
    }
}
