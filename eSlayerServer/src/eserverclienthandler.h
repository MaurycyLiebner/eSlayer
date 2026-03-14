#ifndef ESERVERCLIENTHANDLER_H
#define ESERVERCLIENTHANDLER_H

#include "eserverarea.h"

#include <memory>

struct eUnitsRequest {
    double fTime;
    std::vector<eUnitData> fUnits;
};

class eServerClientHandler {
public:
    eServerClientHandler(const int clientId);

    bool requestUnits();
    bool receiveUnits(std::vector<eUnitData>& units,
                      double& resultTime,
                      const double clientTime);

    void setArea(const std::shared_ptr<eServerArea>& a) { mArea = a; }

    bool moveTo(const ePointF& pos);
    bool attack(const int targetId);
    bool stopAttack();
    bool respawn();
private:
    const int mClientId;
    std::shared_ptr<eServerArea> mArea;
    std::vector<eUnitsRequest> mUnitRequests;
};

#endif // ESERVERCLIENTHANDLER_H
