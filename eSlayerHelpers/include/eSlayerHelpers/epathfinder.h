#ifndef EPATHFINDER_H
#define EPATHFINDER_H

#include "eslayerhelpersexport.h"

#include "epoint.h"

#include <vector>

enum class eTileDirection {
    bottom,
    bottomLeft,
    left,
    topLeft,
    top,
    topRight,
    right,
    bottomRight
};

struct ePathStep {
    ePoint fSrc;
    ePoint fDst;
};

using ePathFinderPath = std::vector<ePathStep>;

class ESLAYERHELPERS_API ePathFinderMap : public std::vector<std::vector<bool>> {
public:
    ePathFinderMap();
    ePathFinderMap(const int w, const int h);

    void set(const ePoint& p, const bool v);
    bool get(const ePoint& p) const;

    int width() const { return mWidth; }
    int height() const { return mHeight; }
private:
    int mWidth = 0;
    int mHeight = 0;
};

class ESLAYERHELPERS_API ePathFinder {
public:
    void setMap(const ePathFinderMap& map);

    ePathFinderPath findPath(const ePoint& from,
                             const ePoint& to,
                             bool& found) const;

private:
    ePathFinderMap mMap;
};

#endif // EPATHFINDER_H
