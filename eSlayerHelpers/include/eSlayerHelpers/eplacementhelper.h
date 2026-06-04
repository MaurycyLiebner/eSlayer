#ifndef EPLACEMENTHELPER_H
#define EPLACEMENTHELPER_H

#include "eslayerhelpersexport.h"

#include <map>

class ESLAYERHELPERS_API ePlacementHelper {
public:
    ePlacementHelper();

    void add(const int id, const int count);
    int get(int& area);
    void set(const int id, const int count);
private:
    std::multimap<int, int> mMap;
};

#endif // EPLACEMENTHELPER_H
