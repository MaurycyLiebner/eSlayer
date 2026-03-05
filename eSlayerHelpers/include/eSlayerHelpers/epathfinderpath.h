#ifndef EPATHFINDERPATH_H
#define EPATHFINDERPATH_H

#include "epoint.h"

#include <deque>

struct ePathStep {
    ePointF fSrc;
    ePointF fDst;
};

class ESLAYERHELPERS_API ePathFinderPath : public std::deque<ePathStep> {
public:
    using std::deque<ePathStep>::deque;

    ePointF posAtDist(const ePointF& start,
                      const double dist) const;
};

#endif // EPATHFINDERPATH_H
