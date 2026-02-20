#include "enamedcheckbox.h"

#include "elabel.h"
#include "echeckbox.h"

void eNamedCheckBox::initialize(const std::string &text) {
    const auto h = new eCheckBox(window());
    h->fitContent();
    addWidget(h);

    const auto n = new eLabel(window());
    n->setText(text);
    n->fitContent();
    addWidget(n);

    const auto res = resolution();
    const int p = res.smallPadding();

    stackHorizontally(p);

    fitContent();
}
