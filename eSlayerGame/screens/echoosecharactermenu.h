#ifndef ECHOOSECHARACTERMENU_H
#define ECHOOSECHARACTERMENU_H

#include "escreenbase.h"

class eCharacters;

class eDialog;

class eChooseCharacterMenu : public eScreenBase {
public:
    eChooseCharacterMenu(eMainWindow* const window);

    using eOkAction = std::function<void(const std::string& name)>;
    using eDeleteAction = std::function<void(const std::string& name)>;
    void initialize(const eAction& exit,
                    const eOkAction& ok,
                    const eAction& createCharacter,
                    const eDeleteAction& deleteCharacter,
                    const eCharacters& chars);
protected:
    bool keyPressEvent(const eKeyPressEvent& e) override;
private:
    void closeDialog();

    eDialog* mDialog = nullptr;
};

#endif // ECHOOSECHARACTERMENU_H
