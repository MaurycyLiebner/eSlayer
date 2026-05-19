#ifndef EPATHFINDER_H
#define EPATHFINDER_H

#include "epathfinderpath.h"
#include "epathfindermap.h"

namespace ePathFinder {
    ESLAYERHELPERS_API
    ePathFinderPath findPath(ePathFinderMap& map,
                             const ePointF& from,
                             const ePointF& to,
                             bool& found);
};

#endif // EPATHFINDER_H
