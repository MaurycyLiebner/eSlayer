#ifndef EPATHFINDERMAP_H
#define EPATHFINDERMAP_H

#include "eslayerhelpersexport.h"

#include "epoint.h"

#include <vector>

class ESLAYERHELPERS_API ePathFinderMap : public std::vector<std::vector<bool>> {
public:
    ePathFinderMap();
    ePathFinderMap(const int x, const int y,
                   const int w, const int h);

    void set(const ePoint& p, const bool v);
    bool get(const ePoint& p) const;

    int width() const { return mWidth; }
    int height() const { return mHeight; }
private:
    int mX = 0;
    int mY = 0;
    int mWidth = 0;
    int mHeight = 0;
};

#endif // EPATHFINDERMAP_H
