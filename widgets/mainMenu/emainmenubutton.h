#ifndef EMAINMENUBUTTON_H
#define EMAINMENUBUTTON_H

#include "../ebuttonbase.h"

class eMainMenuButton : public eButtonBase {
public:
    eMainMenuButton(const std::string& text,
                    eMainWindow* const window);
protected:
    void paintEvent(ePainter& p) override;
};

#endif // EMAINMENUBUTTON_H
