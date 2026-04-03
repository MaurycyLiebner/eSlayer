#ifndef ESETAREAS_H
#define ESETAREAS_H

#include "eslayerhelpersexport.h"

#include "earea.h"

#include <map>
#include <set>

class ESLAYERHELPERS_API eSetAreas {
public:
    eSetAreas(const int areaDim);

    eArea posArea(const ePointF& pos) const;

    void clear();
    bool hasArea(const eArea& area);
    void erase(const eArea& area, const int id);
    void emplace(const eArea& area, const int id);
    const std::set<int>& at(const eArea& area) const;
private:
    const int mAreaDim;
    mutable std::map<eArea, std::set<int>> mAreas;
};

#endif // ESETAREAS_H
