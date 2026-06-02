#ifndef EPATHFINDER_H
#define EPATHFINDER_H

#include "epathfinderpath.h"
#include "epathfindermap.h"

namespace ePathFinder {
    ESLAYERHELPERS_API
    ePathFinderPath findPath(ePathFinderMap& map,
                             const ePointF& from,
                             const std::vector<ePointF>& tos,
                             const int maxDist,
                             bool& found);
};

#endif // EPATHFINDER_H
