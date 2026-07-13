#ifndef ESERVERMOVEMENTHANDLER_H
#define ESERVERMOVEMENTHANDLER_H

#include <eSlayerHelpers/emovementhandlerbase.h>

class eServerUnit;

class eServerMovementHandler :
    public eMovementHandlerBase {
public:
    eServerMovementHandler(
        eServerUnit& u,
        ePathFinderMap& map,
        const int unitTypeId);

    bool increment(const float by);
private:
    eServerUnit& mUnit;

    float mRunSpeedBase = 0.1f;
    float mWalkSpeedBase = 0.07f;
};

#endif // ESERVERMOVEMENTHANDLER_H
