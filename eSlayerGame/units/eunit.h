#ifndef EUNIT_H
#define EUNIT_H

#include "../textures/echarunitmodel.h"

#include <eSlayerHelpers/epoint.h>
#include <eSlayerHelpers/emovementhandler.h>

class eUnit {
public:
    void intialize(const eWalkable& w,
                   const eObsticle& o,
                   const int charId);

    int charId() const { return mCharId; }

    void setPos(const ePointF& pos) { mHandler.setPos(pos); }
    const ePointF& pos() const { return mHandler.pos(); }

    eCharUnitModel& model() { return mModel; }
    const eCharUnitModel& model() const { return mModel; }
    void setModel(const eCharUnitModel& model) { mModel = model; }

    void pushPlanned(const std::queue<eIdPointF>& planned);

    void increment(const double by);
private:
    int mCharId;
    eMovementHandler mHandler;
    eCharUnitModel mModel;
};

#endif // EUNIT_H
