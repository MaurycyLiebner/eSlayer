#ifndef ETCPIPGAMEMENU_H
#define ETCPIPGAMEMENU_H

#include "escreenbase.h"

class eTcpIpGameMenu : public eScreenBase {
public:
    using eScreenBase::eScreenBase;

    void initialize(const eAction& hostGameA,
                    const eAction& joinGameA,
                    const eAction& exitA);
};

#endif // ETCPIPGAMEMENU_H
