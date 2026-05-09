#ifndef ENOVAINCREMENTER_H
#define ENOVAINCREMENTER_H

#include "eslayermissilesexport.h"

#include <cstdint>
#include <functional>
#include <memory>

struct eNova;
class eFixedSizeSetAreas;
struct ePointF;
struct eUnitData;
struct eObject;

class ESLAYERMISSILES_API eNovaIncrementer {
public:
    eNovaIncrementer(eFixedSizeSetAreas& unitAreas);

    using eHasObjects = std::function<bool(
        const int x, const int y)>;
    using eGetObjects = std::function<const std::vector<uint16_t>&(
        const int x, const int y)>;
    using eGetObject = std::function<std::shared_ptr<eObject>(
        const int x, const int y, const uint32_t objectId)>;
    using eRemoveNova = std::function<void(const eNova& n)>;
    using eGetUnit = std::function<eUnitData*(const int charId)>;
    using eHitAction = std::function<void(eUnitData& u)>;
    void initialize(const eHasObjects& hasObjects,
                    const eGetObjects& getObjects,
                    const eGetObject& getObject,
                    const eRemoveNova& removeNova,
                    const eGetUnit& getUnit,
                    const eHitAction& hitAction);

    bool increment(eNova& n, const float by) const;
private:
    eFixedSizeSetAreas& mUnitAreas;
    eHasObjects mHasObjects;
    eGetObjects mGetObjects;
    eGetObject mGetObject;
    eRemoveNova mRemoveNova;
    eGetUnit mGetUnit;
    eHitAction mHitAction;
};

#endif // ENOVAINCREMENTER_H
