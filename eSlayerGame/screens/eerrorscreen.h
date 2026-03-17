#ifndef EERRORSCREEN_H
#define EERRORSCREEN_H

#include "escreenbase.h"

class eErrorScreen : public eScreenBase {
public:
    using eScreenBase::eScreenBase;

    void initialize(const std::string& msg,
                    const std::string& subMsg,
                    const eAction& finish);
protected:
    bool keyPressEvent(const eKeyPressEvent& e) override;
private:
    eAction mFinish;
};

#endif // EERRORSCREEN_H
