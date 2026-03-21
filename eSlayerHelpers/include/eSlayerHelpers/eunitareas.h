#ifndef EUNITAREAS_H
#define EUNITAREAS_H
#include "eslayerhelpersexport.h"

#include "eunitarea.h"

#include <map>
#include <set>

class ESLAYERHELPERS_API eUnitAreas {
public:
    eUnitAreas(const int areaDim);

    eUnitArea posArea(const ePointF& pos) const;

    void clear();
    bool hasArea(const eUnitArea& area);
    void erase(const eUnitArea& area, const int id);
    void emplace(const eUnitArea& area, const int id);
    const std::set<int>& at(const eUnitArea& area);
private:
    const int mAreaDim;
    std::map<eUnitArea, std::set<int>> mAreas;
};

#endif // EUNITAREAS_H
