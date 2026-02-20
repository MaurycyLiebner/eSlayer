#ifndef ECHECKABLEBUTTON_H
#define ECHECKABLEBUTTON_H

#include "ebuttonbase.h"

using eCheckAction = std::function<void(bool)>;

class eCheckableButton : public eButtonBase {
public:
    eCheckableButton(eMainWindow* const window);

    bool checked() const { return mChecked; }
    void setChecked(const bool c) { mChecked = c; }

    void setCheckAction(const eCheckAction& a);
private:
    using eButtonBase::setPressAction;
private:
    bool mChecked = false;
};

#endif // ECHECKABLEBUTTON_H
