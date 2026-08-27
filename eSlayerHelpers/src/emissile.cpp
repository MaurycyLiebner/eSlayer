#include "eSlayerHelpers/emissile.h"

#include "eSlayerHelpers/epacket.h"
#include "eSlayerHelpers/eskills.h"

void eMissile::read(ePacket& p) {
    p >> fId;
    p >> fTeamId;
    p >> fType;
    p >> fPathType;
    p >> fFrom;
    p >> fPos;
    p >> fTo;
    fSpeed = p.readFloatU8(eSkill::sSpeedMax);
    p >> fToPierce;
    p >> fContinuousDamage;
    p >> fEnemyFindRange;
    fRemDist = p.readFloatU8(eSkill::sRangeMax);
    fRemTime = p.readFloatU8(eSkill::sTimeMax);
    fRadius = p.readFloatU8(eSkill::sRadiusMax);
    p >> fDuplicate;

    if(fEnemyFindRange > 0.f) {
        p >> fTwinBehaviour;
        if(fTwinBehaviour != eTwinBehaviour::none) {
            p.read8(fTwinMissiles);
        }
    }
}

void eMissile::write(ePacket& p) const {
    p << fId;
    p << fTeamId;
    p << fType;
    p << fPathType;
    p << fFrom;
    p << fPos;
    p << fTo;
    p.writeFloatU8(fSpeed, eSkill::sSpeedMax);
    p << fToPierce;
    p << fContinuousDamage;
    p << fEnemyFindRange;
    p.writeFloatU8(fRemDist, eSkill::sRangeMax);
    p.writeFloatU8(fRemTime, eSkill::sTimeMax);
    p.writeFloatU8(fRadius, eSkill::sRadiusMax);
    p << fDuplicate;

    if(fEnemyFindRange > 0.f) {
        p << fTwinBehaviour;
        if(fTwinBehaviour != eTwinBehaviour::none) {
            p.write8(fTwinMissiles);
        }
    }
}

bool eMissile::needsUpdate() const {
    return fEnemyFindRange > 0.f;
}

bool eMissile::applyUpdate(const eMissileUpdate& u) {
    if(u.fId != fId) return false;
    fEnemy = u.fEnemy;
    return true;
}

eMissileUpdate eMissile::extractUpdate() const {
    eMissileUpdate result;
    result.fId = fId;
    result.fEnemy = fEnemy;
    return result;
}
