#ifndef EPATHFINDER_H
#define EPATHFINDER_H

#include "epathfinderpath.h"
#include "epathfindermap.h"

namespace ePathFinder {
    ESLAYERHELPERS_API
    ePathFinderPath findPath(const ePathFinderMap& map,
                             const ePoint& from,
                             const ePoint& to,
                             bool& found);
};

#endif // EPATHFINDER_H
