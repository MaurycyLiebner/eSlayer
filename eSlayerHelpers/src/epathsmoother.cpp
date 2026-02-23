#include "../include/eSlayerHelpers/epathsmoother.h"

#include <functional>

using eChecker = std::function<bool(const ePoint&)>;
bool gCheckLineTiles(const ePointF& from,
                     const ePointF& to,
                     const eChecker& checker) {
    const double x0 = from.fX;
    const double y0 = from.fY;
    const double x1 = to.fX;
    const double y1 = to.fY;

    int x = (int)std::floor(x0);
    int y = (int)std::floor(y0);

    const int endX = (int)std::floor(x1);
    const int endY = (int)std::floor(y1);

    const double dx = x1 - x0;
    const double dy = y1 - y0;

    const int stepX = (dx > 0) ? 1 : (dx < 0) ? -1 : 0;
    const int stepY = (dy > 0) ? 1 : (dy < 0) ? -1 : 0;

    const double tDeltaX = (stepX != 0) ? std::abs(1.0 / dx) :
        std::numeric_limits<double>::infinity();
    const double tDeltaY = (stepY != 0) ? std::abs(1.0 / dy) :
        std::numeric_limits<double>::infinity();

    const double nextBoundaryX = (stepX > 0)
                               ? (std::floor(x0) + 1.0)
                               : std::floor(x0);

    const double nextBoundaryY = (stepY > 0)
                               ? (std::floor(y0) + 1.0)
                               : std::floor(y0);

    double tMaxX = (stepX != 0) ? (nextBoundaryX - x0) / dx :
                       std::numeric_limits<double>::infinity();

    double tMaxY = (stepY != 0) ? (nextBoundaryY - y0) / dy :
                       std::numeric_limits<double>::infinity();

    while(true) {
        const bool r = checker({x, y});
        if(!r) return false;

        if(x == endX && y == endY) {
            break;
        }

        if(tMaxX < tMaxY) {
            tMaxX += tDeltaX;
            x += stepX;
        } else {
            tMaxY += tDeltaY;
            y += stepY;
        }
    }
    return true;
}

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

    const auto checker = [&map](const ePoint& p) {
        return map.get(p);
    };
    const bool r = gCheckLineTiles(from, to, checker);
    if(!r) {
        const auto& step = path.front();
        to = step.fDst;
        eVec2d vec{to.fX - from.fX, to.fY - from.fY};
        if(vec.length() > maxDist) {
            vec.normalize(maxDist);
            skipNodes = 0;
        } else {
            skipNodes = 1;
        }
        return vec;
    }

    return eVec2d{to.fX - from.fX, to.fY - from.fY};
}
