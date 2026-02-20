#ifndef ENAMEDCHECKBOX_H
#define ENAMEDCHECKBOX_H

#include "ewidget.h"

class eNamedCheckBox : public eWidget {
public:
    using eWidget::eWidget;

    void initialize(const std::string& text);
};

#endif // ENAMEDCHECKBOX_H
