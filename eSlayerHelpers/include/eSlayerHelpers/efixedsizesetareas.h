#ifndef EFIXEDSIZESETAREAS_H
#define EFIXEDSIZESETAREAS_H

#include "eslayerhelpersexport.h"

#include "earea.h"

#include <set>

class ESLAYERHELPERS_API eFixedSizeSetAreas {
public:
    void initialize(const int width,
                    const int height,
                    const int areaDim);

    eArea posArea(const ePointF& pos) const;
    ePointF areaPos(const eArea& area) const;

    void clear();
    void clear(const eArea& area);
    bool hasArea(const eArea& area) const;
    void erase(const eArea& area, const int id);
    void emplace(const eArea& area, const int id);
    const std::set<int>& at(const eArea& area) const;
private:
    int mWidth;
    int mHeight;
    int mAreaDim;
    std::vector<std::vector<std::set<int>>> mAreas;
};

#endif // EFIXEDSIZESETAREAS_H
