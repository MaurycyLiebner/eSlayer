#ifndef ECREATECHARACTERMENU_H
#define ECREATECHARACTERMENU_H

#include "../widgets/elabel.h"

class eCreateCharacterMenu : public eLabel {
public:
    eCreateCharacterMenu(eMainWindow* const window);

    using eOkAction = std::function<void(const std::string& name,
                                         const bool hardcore)>;
    void initialize(const eAction& exit,
                    const eOkAction& ok);
};

#endif // ECREATECHARACTERMENU_H
