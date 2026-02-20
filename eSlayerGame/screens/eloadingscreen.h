#ifndef ELOADINGSCREEN_H
#define ELOADINGSCREEN_H

#include "escreenbase.h"

class eLoadingScreen : public eScreenBase {
public:
    using eScreenBase::eScreenBase;

    void initialize(const std::vector<eAction>& loading,
                    const eAction& finish);
};

#endif // ELOADINGSCREEN_H
