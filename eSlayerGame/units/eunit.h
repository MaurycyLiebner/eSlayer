#ifndef EUNIT_H
#define EUNIT_H

#include "../textures/echarunitmodel.h"

#include <eSlayerHelpers/eunitdata.h>
#include <eSlayerHelpers/emovementhandler.h>
#include <eSlayerHelpers/epoint.h>

class eUnit : public eUnitData {
public:
    eCharUnitModel& model() { return mModel; }
    const eCharUnitModel& model() const { return mModel; }
    void setModel(const eCharUnitModel& model);

    int bodyAnimId() const { return mBodyAnimId; }

    void setSlayerBody(const bool body) { mSlayerBody = body; }
    bool isSlayerBody() const { return mSlayerBody; }
private:
    int mBodyAnimId = -1;
    bool mSlayerBody = false;
    eCharUnitModel mModel;
};

#endif // EUNIT_H
