#ifndef ECREATECHARACTERMENU_H
#define ECREATECHARACTERMENU_H

#include "escreenbase.h"

class eCreateCharacterMenu : public eScreenBase {
public:
    eCreateCharacterMenu(eMainWindow* const window);
    ~eCreateCharacterMenu();

    using eOkAction = std::function<void(
        const int classId,
        const std::string& name,
        const bool hardcore)>;
    void initialize(const eAction& exit,
                    const eOkAction& ok);
};

#endif // ECREATECHARACTERMENU_H
