#ifndef EUNIT_H
#define EUNIT_H

#include "../textures/echarunitmodel.h"

#include <eSlayerHelpers/eunitdata.h>
#include <eSlayerHelpers/emovementhandler.h>
#include <eSlayerHelpers/epoint.h>

class eUnit : public eUnitData {
public:
    void intialize(const eWalkable& w,
                   const int charId);

    int charId() const { return mCharId; }

    void setPos(const ePointF& pos) { mHandler.setPos(pos); }
    const ePointF& pos() const { return mHandler.pos(); }

    eCharUnitModel& model() { return mModel; }
    const eCharUnitModel& model() const { return mModel; }
    void setModel(const eCharUnitModel& model) { mModel = model; }

    void increment(const double by);

    double actionStartTime() const { return mActStartTime; }
    void setActStartTime(const double t) { mActStartTime = t; }
private:
    int mCharId;
    double mActStartTime = 0.;
    eMovementHandler mHandler;
    eCharUnitModel mModel;
};

#endif // EUNIT_H
