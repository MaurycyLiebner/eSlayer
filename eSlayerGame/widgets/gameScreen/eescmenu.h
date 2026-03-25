#ifndef EESCMENU_H
#define EESCMENU_H

#include "../ewidget.h"

class eESCMenu : public eWidget {
public:
    using eWidget::eWidget;

    void initialize(const eAction& return_,
                    const eAction& exit);
};

#endif // EESCMENU_H
