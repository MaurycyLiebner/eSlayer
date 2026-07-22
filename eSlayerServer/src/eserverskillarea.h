#ifndef ESERVERSKILLAREA_H
#define ESERVERSKILLAREA_H

#include <eSlayerHelpers/eskillarea.h>

#include <functional>

struct eServerSkillArea : public eSkillArea {
    eServerSkillArea();

    static uint32_t sNextId;

    using eAction = std::function<void()>;
    eAction fIncrement;
};

#endif // ESERVERSKILLAREA_H
