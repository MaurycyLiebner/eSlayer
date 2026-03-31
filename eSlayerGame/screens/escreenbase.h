#ifndef ESCREENBASE_H
#define ESCREENBASE_H

#include "../widgets/elabel.h"

class eScreenBase : public eLabel {
public:
    using eLabel::eLabel;

    void setExit(const eAction& exitA);
protected:
    eWidget* addInner();

    bool keyPressEvent(const eKeyPressEvent& e) override;
private:
    eAction mExit;
};

#endif // ESCREENBASE_H
