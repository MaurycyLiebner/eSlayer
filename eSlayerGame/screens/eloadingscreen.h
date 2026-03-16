#ifndef ELOADINGSCREEN_H
#define ELOADINGSCREEN_H

#include "escreenbase.h"

#include <queue>

class eLoadingScreen : public eScreenBase {
public:
    using eScreenBase::eScreenBase;

    void initialize(const std::vector<eAction>& loading,
                    const eAction& finish);
protected:
    void paintEvent(ePainter& p);
private:
    std::queue<eAction> mLoading;
};

#endif // ELOADINGSCREEN_H
