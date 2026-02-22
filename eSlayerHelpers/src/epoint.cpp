#include "../include/eSlayerHelpers/epoint.h"

double ePointF::distance(const ePointF& p1,
                         const ePointF& p2) {
    const double dx = p1.fX - p2.fX;
    const double dy = p1.fY - p2.fY;
    return std::sqrt(dx*dx + dy*dy);
}
