#include "../include/eSlayerHelpers/epoint.h"

float ePointF::distance(const ePointF& p1,
                         const ePointF& p2) {
    const float dx = p1.fX - p2.fX;
    const float dy = p1.fY - p2.fY;
    return std::sqrt(dx*dx + dy*dy);
}

eVec2f ePointF::vector(const ePointF& to, const ePointF& from) {
    return eVec2f{to.fX - from.fX, to.fY - from.fY};
}
