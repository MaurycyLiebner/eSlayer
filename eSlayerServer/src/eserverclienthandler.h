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
    bool requestUnits();
    bool receiveUnits(std::vector<eUnitData>& units,
                      double& resultTime,
                      const double clientTime);

    void setArea(const std::shared_ptr<eServerArea>& a) { mArea = a; }

    bool moveTo(const int clientId, const ePointF& pos);
    bool attack(const int clientId, const int targetId);
    bool stopAttack(const int clientId);
private:
    int mDelay = 100;
    std::shared_ptr<eServerArea> mArea;
    std::vector<eUnitsRequest> mUnitRequests;
};

#endif // ESERVERCLIENTHANDLER_H
