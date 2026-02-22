#ifndef EESCMENUBUTTON_H
#define EESCMENUBUTTON_H

#include "../ebuttonbase.h"

class eESCMenuButton : public eButtonBase {
public:
    eESCMenuButton(const std::string& text,
                   eMainWindow* const window);
protected:
    void paintEvent(ePainter& p) override;
};

#endif // EESCMENUBUTTON_H
