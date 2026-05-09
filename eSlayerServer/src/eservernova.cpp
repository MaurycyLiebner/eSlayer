#include "eservernova.h"

uint32_t eServerNova::sNextId = 1;

eServerNova::eServerNova() {
    fId = sNextId++;
}
