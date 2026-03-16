#ifndef EATTACKDATA_H
#define EATTACKDATA_H

#include "eSlayerHelpers/epoint.h"
#include "eSlayerHelpers/epacket.h"

#include <cstdint>

enum class eAttackTargetType {
    none, character, position
};

struct eAttackData {
    eAttackData() :
        fType(eAttackTargetType::none) {}
    eAttackData(const int c) :
        fType(eAttackTargetType::character),
        fChar(c) {}
    eAttackData(const ePointF& pos) :
        fType(eAttackTargetType::position),
        fPos(pos) {}

    eAttackTargetType fType;
    int32_t fChar;
    ePointF fPos;

    void read(ePacket& p) {
        p >> fType;
        switch(fType) {
        case eAttackTargetType::character:
            p >> fChar;
        case eAttackTargetType::position:
            p >> fPos;
        }
    }

    void write(ePacket& p) const {
        p << fType;
        switch(fType) {
        case eAttackTargetType::character:
            p << fChar;
        case eAttackTargetType::position:
            p << fPos;
        }
    }
};

#endif // EATTACKDATA_H
