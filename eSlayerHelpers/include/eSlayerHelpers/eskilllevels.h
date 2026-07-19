#ifndef ESKILLLEVELS_H
#define ESKILLLEVELS_H

#include "eslayerhelpersexport.h"

#include <map>
#include <cstdint>

class ePacket;

struct ESLAYERHELPERS_API eSkillLevels :
        public std::map<uint16_t, uint16_t> {
    eSkillLevels();

    uint8_t fRemainingPoints = 0;

    void read(ePacket& p);
    void write(ePacket& p) const;

    int skillLevel(const int skillId) const;
    void incSkillLevels(const int by);
    void incClassSkillLevels(const int classId, const int by);
    void incSkillLevel(const int by, const int skillId);
};

#endif // ESKILLLEVELS_H
