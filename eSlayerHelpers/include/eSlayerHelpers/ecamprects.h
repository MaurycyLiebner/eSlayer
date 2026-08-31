#ifndef ECAMPRECTS_H
#define ECAMPRECTS_H

#include "eslayerhelpersexport.h"

#include "erect.h"

class ESLAYERHELPERS_API eCampRects {
public:
    bool campAt(const ePointF& pos) const;
    bool campAtLine(const ePointF& from,
                    const ePointF& to) const;
    void addRect(const eRectF& rect);
private:
    std::vector<eRectF> mRects;
};

#endif // ECAMPRECTS_H
