#include "escreenbase.h"

#include "../widgets/mainMenu/emainmenubutton.h"
#include "../widgets/mainMenu/edialog.h"
#include "../etext.h"

void eScreenBase::setExit(const eAction& exitA) {
    mExit = exitA;
}

eWidget* eScreenBase::addInner() {
    const auto frame = new eWidget(window());
    addWidget(frame);

    const auto& res = resolution();

    const int p = res.largePadding();
    const int cww = res.centralWidgetLargeWidth();
    const int cwh = res.centralWidgetLargeHeight();
    frame->resize(cww, cwh);

    frame->align(eAlignment::center);

    const auto inner = new eWidget(window());
    inner->setNoPadding();
    frame->addWidget(inner);
    inner->move(2*p, 2*p);
    inner->resize(cww - 4*p, cwh - 4*p);

    return inner;
}

bool eScreenBase::keyPressEvent(const eKeyPressEvent& e) {
    if(e.key() == SDL_SCANCODE_ESCAPE) {
        if(mDialog) {
            closeDialog();
            return true;
        }
        if(mExit) {
            mExit();
            return true;
        }
    }
    return false;
}

eDialog* eScreenBase::showDialog(
    const std::string& text,
    const eAction& yesAction,
    const eAction& noAction,
    const eAction& okAction) {
    if(mDialog) return nullptr;
    mDialog = new eDialog(window());

    const int w = dialogWidth();

    const auto ask = new eLabel(window());
    ask->setSmallFontSize();
    ask->setWrapWidth(w);
    ask->setText(text);
    ask->fitHeight();
    ask->setWidth(w);
    mDialog->addWidget(ask);

    const auto buttonsW = new eWidget(window());
    buttonsW->setNoPadding();

    if(yesAction) {
        const auto yes = new eMainMenuButton(
            eText::text(3, 5), window());
        yes->fitContent();
        yes->setPressAction(yesAction);
        buttonsW->addWidget(yes);
    }

    if(noAction) {
        const auto no = new eMainMenuButton(
            eText::text(3, 6), window());
        no->fitContent();
        no->setPressAction(noAction);
        buttonsW->addWidget(no);
    }

    if(okAction) {
        const auto ok = new eMainMenuButton(
            eText::text(1, 5), window());
        ok->fitContent();
        ok->setPressAction(okAction);
        buttonsW->addWidget(ok);
    }

    buttonsW->setWidth(w);
    buttonsW->layoutHorizontallyWithoutSpaces();
    buttonsW->fitContent();
    mDialog->addWidget(buttonsW);

    mDialog->stackVertically();
    mDialog->fitContent();

    addWidget(mDialog);
    mDialog->align(eAlignment::center);
    buttonsW->align(eAlignment::hcenter);

    return mDialog;
}

int eScreenBase::dialogWidth() const {
    return eWidget::width()/3;
}

void eScreenBase::closeDialog() {
    if(!mDialog) return;
    mDialog->deleteLater();
    mDialog = nullptr;
}
