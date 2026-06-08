#ifndef ESERVERMOVEMENTHANDLER_H
#define ESERVERMOVEMENTHANDLER_H

#include <eSlayerHelpers/emovementhandlerbase.h>

class eServerUnit;

class eServerMovementHandler :
    public eMovementHandlerBase {
public:
    eServerMovementHandler(
        eServerUnit& u,
        ePathFinderMap& map);

    bool increment(const float by);
private:
    eServerUnit& mUnit;
};

#endif // ESERVERMOVEMENTHANDLER_H
