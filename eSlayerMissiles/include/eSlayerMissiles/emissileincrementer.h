#ifndef EMISSILEINCREMENTER_H
#define EMISSILEINCREMENTER_H

#include "eslayermissilesexport.h"

#include <cstdint>
#include <functional>

struct eMissile;
class eFixedSizeSetAreas;
struct ePointF;
struct eUnitData;

class ESLAYERMISSILES_API eMissileIncrementer {
public:
    eMissileIncrementer(eFixedSizeSetAreas& unitAreas);

    using eObstacle = std::function<bool(const ePointF& pos)>;
    using eRemoveMissile = std::function<void(eMissile& m)>;
    using eGetUnit = std::function<eUnitData*(const uint32_t charId)>;
    using eHitAction = std::function<void(const eMissile& m, eUnitData& u)>;
    void initialize(const eObstacle& obstacle,
                    const eRemoveMissile& removeMissile,
                    const eGetUnit& getUnit,
                    const eHitAction& hitAction);

    bool increment(eMissile& m, const float by) const;
private:
    eFixedSizeSetAreas& mUnitAreas;
    eObstacle mObstacle;
    eRemoveMissile mRemoveMissile;
    eGetUnit mGetUnit;
    eHitAction mHitAction;
};

#endif // EMISSILEINCREMENTER_H
