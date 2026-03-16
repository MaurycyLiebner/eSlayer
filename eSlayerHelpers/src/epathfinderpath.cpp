#include "../include/eSlayerHelpers/epathfinderpath.h"

#include "../include/eSlayerHelpers/evec2.h"

ePointF ePathFinderPath::posAtDist(
    const ePointF& start,
    const float dist) const {
    if(empty()) return start;
    float remDist = dist;
    int nodeId = 0;
    const auto& first = operator[](nodeId++);
    auto pos = start;
    auto dst = first;
    while(remDist > 0) {
        eVec2f vec(dst.fX - pos.fX, dst.fY - pos.fY);
        const float newRem = remDist - vec.length();
        if(vec.length() > remDist) vec.normalize(remDist);
        pos.fX += vec.x;
        pos.fY += vec.y;
        if(nodeId >= size()) break;
        const auto& next = operator[](nodeId++);
        dst = next;
        remDist = newRem;
    }
    return pos;
}
