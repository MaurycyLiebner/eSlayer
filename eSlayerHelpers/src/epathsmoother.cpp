#include "../include/eSlayerHelpers/epathsmoother.h"

eVec2d ePathSmoother::moveDir(
    const ePathFinderPath& path,
    const ePathFinderMap& map,
    const ePointF& from,
    const double maxDist,
    int& skipNodes) {
    skipNodes = 0;
    ePointF to = from;
    double checkDistMin = 0.;
    double checkDistMax = 4*maxDist;
    while(checkDistMax - checkDistMin > 0.01) {
        const double middleDist = (checkDistMax + checkDistMin)/2;
        to = path.posAtDist(from, middleDist, skipNodes);
        const double dist = ePointF::distance(from, to);
        if(dist > maxDist) {
            checkDistMax = middleDist;
        } else {
            checkDistMin = middleDist;
        }
    }
    return eVec2d{to.fX - from.fX, to.fY - from.fY};
}
