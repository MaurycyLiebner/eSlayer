#ifndef EAREAS_H
#define EAREAS_H

#include "earea.h"

#include <set>

class ESLAYERHELPERS_API eAreas {
public:
    eAreas(const int areaDim);

    eArea posArea(const ePointF& pos) const;
    ePointF areaPos(const eArea& area) const;

    void clear();
    bool hasArea(const eArea& area);
    void erase(const eArea& area);
    void emplace(const eArea& area);
private:
    const int mAreaDim;
    mutable std::set<eArea> mAreas;
};

#endif // EAREAS_H
