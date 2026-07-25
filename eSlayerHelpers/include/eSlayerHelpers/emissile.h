#ifndef EMISSILE_H
#define EMISSILE_H

#include "eslayerhelpersexport.h"

#include "epositioned.h"

#include "eteamid.h"

#include <set>

class ePacket;

struct ESLAYERHELPERS_API eMissileUpdate {
    uint32_t fId;
    uint32_t fEnemy;
};

enum class eTwinBehaviour : uint8_t {
    none, alwaysAvoid, tryAvoid
};

struct ESLAYERHELPERS_API eMissile :
        public ePositioned {
    uint32_t fId;
    eTeamId fTeamId;
    uint8_t fType;
    uint8_t fPathType;
    ePointF fFrom;
    ePointF fTo;
    float fSpeed;
    // number of obstacles missile can hit (0 - infinity)
    uint8_t fToPierce;
    bool fContinuousDamage;
    float fRemDist;
    float fRemTime;
    float fRadius;
    float fEnemyFindRange = 0.f;
    uint32_t fEnemy = 0;
    ePointF fEnemyPos;
    float fTime = 0.f;

    eTwinBehaviour fTwinBehaviour = eTwinBehaviour::none;
    std::set<uint32_t> fTwinMissiles;
    std::set<uint32_t> fPierced;

    void read(ePacket& p);
    void write(ePacket& p) const;

    bool needsUpdate() const;
    bool applyUpdate(const eMissileUpdate& u);
    eMissileUpdate extractUpdate() const;
};

#endif // EMISSILE_H
