#ifndef EMISSILEINCREMENTER_H
#define EMISSILEINCREMENTER_H

#include "eslayermissilesexport.h"

#include <functional>

struct eMissile;
class eFixedSizeSetAreas;
struct ePointF;
struct eUnitData;

class ESLAYERMISSILES_API eMissileIncrementer {
public:
    eMissileIncrementer(eFixedSizeSetAreas& unitAreas);

    using eObsticle = std::function<bool(const ePointF& pos)>;
    using eRemoveMissile = std::function<void(eMissile& m)>;
    using eGetUnit = std::function<eUnitData*(const int charId)>;
    using eHitAction = std::function<void(const eMissile& m, eUnitData& u)>;
    void initialize(const eObsticle& obsticle,
                    const eRemoveMissile& removeMissile,
                    const eGetUnit& getUnit,
                    const eHitAction& hitAction);

    bool increment(eMissile& m, const float by) const;
private:
    eFixedSizeSetAreas& mUnitAreas;
    eObsticle mObsticle;
    eRemoveMissile mRemoveMissile;
    eGetUnit mGetUnit;
    eHitAction mHitAction;
};

#endif // EMISSILEINCREMENTER_H
