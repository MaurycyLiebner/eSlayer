#ifndef ENOVAINCREMENTER_H
#define ENOVAINCREMENTER_H

#include "eslayermissilesexport.h"

#include <functional>
#include <memory>
#include <cstdint>

struct eNova;
class eFixedSizeSetAreas;
struct ePointF;
struct eUnitData;
struct eObject;
struct eTile;
struct eArcIntervals;

class ESLAYERMISSILES_API eNovaIncrementer {
public:
    eNovaIncrementer(eFixedSizeSetAreas& unitAreas);

    using eTileInside = std::function<bool(
        const int x, const int y)>;
    using eGetObjects = std::function<const std::vector<int>&(
        const int x, const int y)>;
    using eGetObject = std::function<std::shared_ptr<eObject>(
        const int objectId)>;
    using eGetTile = std::function<const eTile&(
        const int x, const int y)>;
    using eRemoveNova = std::function<void(const eNova& n)>;
    using eGetUnit = std::function<eUnitData*(const uint32_t charId)>;
    using eHitAction = std::function<void(
        eNova& n, eUnitData& u)>;
    void initialize(const eTileInside& tileInside,
                    const eGetObjects& getObjects,
                    const eGetObject& getObject,
                    const eGetTile& getTile,
                    const eRemoveNova& removeNova,
                    const eGetUnit& getUnit,
                    const eHitAction& hitAction);

    bool increment(eNova& n, const float by) const;
private:
    eFixedSizeSetAreas& mUnitAreas;
    eTileInside mTileInside;
    eGetObjects mGetObjects;
    eGetObject mGetObject;
    eGetTile mGetTile;
    eRemoveNova mRemoveNova;
    eGetUnit mGetUnit;
    eHitAction mHitAction;
};

#endif // ENOVAINCREMENTER_H
