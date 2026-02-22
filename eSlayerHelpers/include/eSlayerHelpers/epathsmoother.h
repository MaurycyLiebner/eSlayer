#ifndef EPATHSMOOTHER_H
#define EPATHSMOOTHER_H

#include "epathfinderpath.h"
#include "epathfindermap.h"
#include "evec2.h"

namespace ePathSmoother {
    ESLAYERHELPERS_API
    eVec2d moveDir(const ePathFinderPath& path,
                   const ePathFinderMap& map,
                   const ePointF& from,
                   const double maxDist,
                   int& skipNodes);
};

#endif // EPATHSMOOTHER_H
