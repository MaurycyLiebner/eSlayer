#ifndef EPATHFINDERPATH_H
#define EPATHFINDERPATH_H

#include "epoint.h"

#include <vector>

struct ePathStep {
    ePointF fSrc;
    ePointF fDst;
};

class ESLAYERHELPERS_API ePathFinderPath : public std::vector<ePathStep> {
public:
    using std::vector<ePathStep>::vector;

    ePointF posAtDist(const ePointF& start,
                      const double dist) const;
};

#endif // EPATHFINDERPATH_H
