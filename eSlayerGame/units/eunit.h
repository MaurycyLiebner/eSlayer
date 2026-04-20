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
    void setModel(const eCharUnitModel& model) { mModel = model; }

    void setBody(const bool body) { mBody = body; }
    bool isBody() const { return mBody; }
private:
    bool mBody = false;
    eCharUnitModel mModel;
};

#endif // EUNIT_H
