#include "echoosecharacterwidget.h"

#include "../../echaracters.h"
#include "echaracterbutton.h"

void eChooseCharacterWidget::initialize(
    const int totWidth,
    const int buttHeight,
    const eCharacters& chars) {
    const auto& chars2 = chars.get();

    const auto res = resolution();

    const int w1 = totWidth/2;
    const int w2 = totWidth - w1;

    eWidget* row = nullptr;
    const int iMax = chars2.size();
    const auto current = std::make_shared<eCharacterButton*>(nullptr);
    const auto setCurrent = [this, current](eCharacterButton* const b) {
        b->setChecked(true);
        if(*current && *current != b) {
            (*current)->setChecked(false);
        }
        mCurrent = b->text();
        *current = b;
    };
    for(int i = 0; i < iMax; i++) {
        if(i % 2 == 0) row = nullptr;
        if(!row) {
            row = new eWidget(window());
            row->setNoPadding();
            addWidget(row);
        }

        const auto b = new eCharacterButton(window());
        const auto& c = chars2[i];
        b->initialize(c);
        b->setWidth(i % 2 ? w1 : w2);
        b->setHeight(buttHeight);
        const auto name = c.name();
        b->setCheckAction([setCurrent, b](const bool c) {
            if(!c) {
                b->setChecked(true);
                return;
            }
            setCurrent(b);
        });
        row->addWidget(b);

        row->stackHorizontally();
        row->fitContent();

        if(i == 0) setCurrent(b);
    }

    stackVertically();
    fitHeight();
}
