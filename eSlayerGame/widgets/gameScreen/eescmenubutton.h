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

class eESCMenuSwitchButton : public eESCMenuButton {
public:
    using eSwitchAction = std::function<void(const int id)>;
    eESCMenuSwitchButton(const std::string& mainText,
                         const std::vector<std::string>& values,
                         const int iniId,
                         const eSwitchAction& switchA,
                         eMainWindow* const window,
                         const int width);
private:
    int mId = 0;
    eLabel* mValue = nullptr;
};

#endif // EESCMENUBUTTON_H
