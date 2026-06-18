#include "ecoinsquestionwidget.h"

#include "../elabel.h"
#include "../../etext.h"
#include "../elineedit.h"
#include "eokcancelbutton.h"
#include "../../emainwindow.h"

eCoinsQuestionWidget::~eCoinsQuestionWidget() {
    const auto window = eWidget::window();
    window->stopTextInput();
}

uint32_t stringToUInt(const std::string& str) {
    try {
        return std::stoi(str);
    } catch(...) {
        return 0;
    }
}

void eCoinsQuestionWidget::initialize(
    const eGoldAction& a,
    const int s, const int max) {
    const auto window = eWidget::window();
    window->startTextInput();

    const auto innerW = new eWidget(window);
    innerW->setNoPadding();

    const auto& res = resolution();
    const float mult = res.multiplier();
    const auto qlabel = new eLabel(window);
    qlabel->setWrapWidth(300*mult);
    qlabel->setText(eText::text(18, s));
    qlabel->setTextAlignment(eAlignment::hcenter);
    qlabel->fitContent();
    innerW->addWidget(qlabel);

    const auto line = new eLineEdit(window);
    line->setChangeAction([line, max]() {
        const auto str = line->text();
        const uint32_t v = stringToUInt(str);
        if(v > max) {
            line->setText(std::to_string(max));
        }
    });
    line->setAllowed("0123456789");
    line->setText(std::to_string(max));
    line->fitContent();
    innerW->addWidget(line);

    const auto buttonsW = new eWidget(window);
    buttonsW->setNoPadding();
    const int w = qlabel->width();
    buttonsW->setWidth(w);

    const auto okB = new eOkButton(window);
    okB->setCheckAction([this, line, a](const bool) {
        const auto str = line->text();
        const uint32_t v = stringToUInt(str);
        if(v > 0) a(v);
        deleteLater();
    });
    buttonsW->addWidget(okB);

    const auto cancelB = new eCancelButton(window);
    cancelB->setCheckAction([this](const bool) {
        deleteLater();
    });
    buttonsW->addWidget(cancelB);

    buttonsW->layoutHorizontallyWithoutSpaces();
    buttonsW->fitContent();

    innerW->addWidget(buttonsW);

    const int p = res.largePadding();
    innerW->stackVertically(p);
    innerW->fitContent();
    line->align(eAlignment::hcenter);

    setup(innerW);
}

bool eCoinsQuestionWidget::mouseMoveEvent(
    const eMouseEvent& e) {
    return true;
}
