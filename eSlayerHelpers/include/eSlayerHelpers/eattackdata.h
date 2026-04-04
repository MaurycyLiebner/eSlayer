#ifndef EATTACKDATA_H
#define EATTACKDATA_H

#include "eSlayerHelpers/epoint.h"
#include "eSlayerHelpers/epacket.h"
#include "eSlayerHelpers/eskillchoice.h"

#include <cstdint>

enum class eAttackTargetType {
    none, character, position
};

struct eAttackData {
    eAttackData() :
        fType(eAttackTargetType::none) {}
    eAttackData(const int c,
                const eSkillChoice skill) :
        fType(eAttackTargetType::character),
        fSkill(static_cast<int>(skill)),
        fChar(c) {}
    eAttackData(const ePointF& pos,
                const eSkillChoice skill) :
        fType(eAttackTargetType::position),
        fSkill(static_cast<int>(skill)),
        fPos(pos) {}
    eAttackData(const int c,
                const int skill) :
        fType(eAttackTargetType::character),
        fSkill(skill),
        fChar(c) {}
    eAttackData(const ePointF& pos,
                const int skill) :
        fType(eAttackTargetType::position),
        fSkill(skill),
        fPos(pos) {}

    eAttackTargetType fType;
    int fSkill;
    int32_t fChar;
    ePointF fPos;

    void read(ePacket& p) {
        p >> fType;
        p >> fSkill;
        switch(fType) {
        case eAttackTargetType::character:
            p >> fChar;
        case eAttackTargetType::position:
            p >> fPos;
        case eAttackTargetType::none:
            break;
        }
    }

    void write(ePacket& p) const {
        p << fType;
        p << fSkill;
        switch(fType) {
        case eAttackTargetType::character:
            p << fChar;
        case eAttackTargetType::position:
            p << fPos;
        case eAttackTargetType::none:
            break;
        }
    }
};

#endif // EATTACKDATA_H
