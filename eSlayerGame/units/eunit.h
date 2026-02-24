#ifndef EUNIT_H
#define EUNIT_H

#include "../textures/echarunitmodel.h"

#include <eSlayerHelpers/epoint.h>

class eUnit {
public:
    eUnit(const int id);

    int charId() const { return mCharId; }

    void setPos(const ePointF& pos) { mPos = pos; }
    const ePointF& pos() const { return mPos; }

    eCharUnitModel& model() { return mModel; }
    const eCharUnitModel& model() const { return mModel; }
    void setModel(const eCharUnitModel& model) { mModel = model; }
private:
    const int mCharId;
    ePointF mPos;
    eCharUnitModel mModel;
};

#endif // EUNIT_H
