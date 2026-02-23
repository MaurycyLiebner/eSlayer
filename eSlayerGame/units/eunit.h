#ifndef EUNIT_H
#define EUNIT_H

#include "../textures/echarunitmodel.h"

#include <eSlayerHelpers/epoint.h>

class eUnit {
public:
    eUnit();

    void setPos(const ePointF& pos) { mPos = pos; }
    const ePointF& pos() const { return mPos; }

    eCharUnitModel& model() { return mModel; }
    const eCharUnitModel& model() const { return mModel; }
    void setModel(const eCharUnitModel& model) { mModel = model; }
private:
    ePointF mPos;
    eCharUnitModel mModel;
};

#endif // EUNIT_H
