#ifndef ESERVERCLIENTHANDLER_H
#define ESERVERCLIENTHANDLER_H

#include "eSlayerServer/eserverunit.h"
#include "eserverarea.h"

#include <chrono>
using namespace std::chrono;

#include <memory>

struct eUnitsRequest {
    milliseconds fRequestTime;
    std::vector<std::shared_ptr<eServerUnit>> fUnits;
};

class eServerClientHandler {
public:
    bool requestUnits();
    int receiveUnits(std::vector<std::shared_ptr<eServerUnit>>& units);

    void setArea(const std::shared_ptr<eServerArea>& a) { mArea = a; }
private:
    int mDelay = 100;
    std::shared_ptr<eServerArea> mArea;
    std::vector<eUnitsRequest> mUnitRequests;
};

#endif // ESERVERCLIENTHANDLER_H
