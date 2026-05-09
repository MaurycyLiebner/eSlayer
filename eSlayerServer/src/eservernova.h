#ifndef ESERVERNOVA_H
#define ESERVERNOVA_H

#include <eSlayerHelpers/enova.h>

#include <functional>

class eServerUnit;

struct eServerNova : public eNova {
    eServerNova();

    static uint32_t sNextId;

    using eHitAction = std::function<void(eServerUnit&)>;
    eHitAction fHitAction;
};

#endif // ESERVERNOVA_H
