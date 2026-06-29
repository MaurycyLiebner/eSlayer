#include "eSlayerHelpers/eobject.h"

#include "eSlayerHelpers/erect.h"

bool eObject::inside(const ePointF& pos) const {
    const eRectF rect{fPos.fX, fPos.fY, fWidth, fHeight};
    return rect.inside(pos);
}
