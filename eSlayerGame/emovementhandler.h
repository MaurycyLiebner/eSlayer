#ifndef EMOVEMENTHANDLER_H
#define EMOVEMENTHANDLER_H

#include <eSlayerHelpers/emovementhandlerbase.h>

class eMovementHandler :
    public eMovementHandlerBase {
public:
    eMovementHandler(eUnitData& u);

    bool increment(const float by);
private:
    eUnitData& mUnit;
};

#endif // EMOVEMENTHANDLER_H
