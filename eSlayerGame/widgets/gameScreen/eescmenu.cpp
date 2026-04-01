#include "eescmenu.h"

#include "eescmenubutton.h"
#include "../../elanguage.h"

void eESCMenu::initialize(const eAction& return_,
                          const eAction& exit) {
    const auto optionsB = new eESCMenuButton(
        eLanguage::text(5, 0), window());
    addWidget(optionsB);

    const auto exitB = new eESCMenuButton(
        eLanguage::text(5, 1), window());
    addWidget(exitB);
    exitB->setPressAction([exit]() {
        if(exit) exit();
    });

    const auto returnB = new eESCMenuButton(
        eLanguage::text(5, 2), window());
    addWidget(returnB);
    returnB->setPressAction([return_]() {
        if(return_) return_();
    });

    const auto& res = resolution();
    const int p = res.hugePadding();
    stackVertically(p);
    fitContent();

    optionsB->align(eAlignment::hcenter);
    exitB->align(eAlignment::hcenter);
    returnB->align(eAlignment::hcenter);
}
