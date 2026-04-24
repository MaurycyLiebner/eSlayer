#ifndef EMISSILE_H
#define EMISSILE_H

#include "eslayerhelpersexport.h"

#include "epositioned.h"

#include "eSlayerHelpers/eteamid.h"

#include <set>

class ePacket;

struct ESLAYERHELPERS_API eMissile : public ePositioned {
    int32_t fId;
    eTeamId fTeamId;
    uint8_t fType;
    uint8_t fPathType;
    ePointF fFrom;
    ePointF fTo;
    float fSpeed;
    // number of obsticles missile can hit (0 - infinity)
    uint8_t fToPierce;
    bool fContinuousDamage;
    float fRemDistTime;
    float fRadius;
    float fEnemyFindRange = 0.f;
    bool fEnemy = false;
    ePointF fEnemyPos;
    float fTime = 0.f;

    std::set<int32_t> fPierced;

    void read(ePacket& p);
    void write(ePacket& p) const;
};

#endif // EMISSILE_H
