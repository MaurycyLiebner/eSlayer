#ifndef EPATHFINDERPATH_H
#define EPATHFINDERPATH_H

#include "epoint.h"

#include <deque>

class ESLAYERHELPERS_API ePathFinderPath : public std::deque<ePointF> {
public:
    using std::deque<ePointF>::deque;

    ePointF posAtDist(const ePointF& start,
                      const double dist) const;
};

#endif // EPATHFINDERPATH_H
