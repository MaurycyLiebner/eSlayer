#ifndef ETCPIPJOINMENU_H
#define ETCPIPJOINMENU_H

#include "escreenbase.h"

using eJoinAction = std::function<void(const std::string& ip)>;

class eTcpIpJoinMenu : public eScreenBase {
public:
    using eScreenBase::eScreenBase;

    void initialize(const eJoinAction& joinGameA,
                    const eAction& exitA);
};

#endif // ETCPIPJOINMENU_H
