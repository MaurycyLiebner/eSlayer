#ifndef ESERVERSKILLAREA_H
#define ESERVERSKILLAREA_H

#include <eSlayerHelpers/eskillarea.h>

struct eServerSkillArea : public eSkillArea {
    eServerSkillArea();

    static uint32_t sNextId;
};

#endif // ESERVERSKILLAREA_H
