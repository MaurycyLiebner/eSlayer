#ifndef ESCREENBASE_H
#define ESCREENBASE_H

#include "../widgets/elabel.h"

class eDialog;

class eScreenBase : public eLabel {
public:
    using eLabel::eLabel;

    void setExit(const eAction& exitA);

    void showDialog(
        const std::string& text,
        const eAction& yesAction,
        const eAction& noAction,
        const eAction& okAction);
    void closeDialog();
protected:
    eWidget* addInner();

    bool keyPressEvent(const eKeyPressEvent& e) override;
private:
    eDialog* mDialog = nullptr;
    eAction mExit;
};

#endif // ESCREENBASE_H
