#include "../include/eSlayerHelpers/epathfinderpath.h"

#include "../include/eSlayerHelpers/evec2.h"

ePointF ePathFinderPath::posAtDist(
    const double dist, int& skipNodes) const {
    const auto& first = operator[](0);
    const auto& pos = first.fSrc;
    return posAtDist(pos, dist, skipNodes);
}

ePointF ePathFinderPath::posAtDist(
    const ePointF& start,
    const double dist,
    int& skipNodes) const {
    skipNodes = 0;
    if(empty()) return start;
    double rem = dist;
    int nodeId = -1;
    const auto& first = operator[](++nodeId);
    auto pos = start;
    auto dst = first.fDst;
    while(rem > 0) {
        skipNodes = nodeId;
        eVec2d vec(dst.fX - pos.fX, dst.fY - pos.fY);
        const double newRem = rem - vec.length();
        if(vec.length() > rem) vec.normalize(rem);
        else skipNodes++;
        pos.fX += vec.x;
        pos.fY += vec.y;
        if(nodeId + 1 >= size()) break;
        const auto& next = operator[](++nodeId);
        dst = next.fDst;
        rem = newRem;
    }
    return pos;
}
