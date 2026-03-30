#ifndef ESTATS_H
#define ESTATS_H

#include "eslayerhelpersexport.h"

#include "edamage.h"
#include "eweapontype.h"

#include <map>

struct eEquipment;
struct eAttributes;

struct ESLAYERHELPERS_API eStats {
    float fAttackRatingL = 100.f;
    float fAttackRatingR = 100.f;
    float fDefense = 100.f;
    float fBlockChance = 0.f;
    float fFasterHitRecovery = 0.f;
    float fFasterBlockRate = 0.f;
    float fWalkRun = 0.f;

    float fHealthF = 100.f;
    float fMaxHealth = 100.f;

    float fManaF = 100.f;
    float fMaxMana = 100.f;

    // LWLS - left weapon, left skill
    float fAttackSpeedLW = 0.f;
    float fAttackSpeedRW = 0.f;
    float fAttackSpeedLS = 0.f;
    float fAttackSpeedRS = 0.f;

    float fCastRate = 0.f;

    float fWeaponMeeleRange = 0.f;
    float fWeaponRangedRange = 0.f;

    eWeaponType fWeaponTypeL = eWeaponType::meele;
    eWeaponType fWeaponTypeR = eWeaponType::meele;

    int fMissileIdLWLS = -1;
    int fMissileIdRWLS = -1;
    int fMissileIdLWRS = -1;
    int fMissileIdRWRS = -1;

    // LWLS - left weapon, left skill
    eDamage fDamageMinLWLS;
    eDamage fDamageMaxLWLS;
    eDamage fDamageMinRWLS;
    eDamage fDamageMaxRWLS;

    eDamage fDamageMinLWRS;
    eDamage fDamageMaxLWRS;
    eDamage fDamageMinRWRS;
    eDamage fDamageMaxRWRS;

    std::map<int, float> fCooldowns;
    std::map<int, int> fSkillLevels;

    int fSkillL = -1;
    int fSkillR = -1;

    int fMissilesL = 0;
    int fMissilesR = 0;

    float fPierceL = 0.f;
    float fPierceR = 0.f;

    void calculate(const eAttributes& attr,
                   const eEquipment& eq);
};

#endif // ESTATS_H
