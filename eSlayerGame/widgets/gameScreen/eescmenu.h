#ifndef EESCMENU_H
#define EESCMENU_H

#include "../ewidget.h"

class eESCMenu : public eWidget {
public:
    using eWidget::eWidget;

    void initialize(const eAction& return_,
                    const eAction& exit);
private:
    int buttonMaxWidth() const;

    void showOptions();
    void showSoundOptions();
    void showVideoOptions();

    eAction mReturn;
    eAction mExit;
};

#endif // EESCMENU_H
