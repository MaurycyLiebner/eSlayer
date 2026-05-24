#include "eserverskillarea.h"

uint32_t eServerSkillArea::sNextId = 1;

eServerSkillArea::eServerSkillArea() {
    fId = sNextId++;
}
