#ifndef ESKILLSELECTWIDGET_H
#define ESKILLSELECTWIDGET_H

#include "../ewidget.h"

#include <eSlayerHelpers/eskillchoice.h>

struct eSkillSelect {
    int fSkillId;
    bool fCanUse;
};

class eSkillSelectWidget : public eWidget {
public:
    using eWidget::eWidget;

    using eChooseAction = std::function<void(int)>;
    void initialize(const std::vector<eSkillSelect>& skillIds,
                    const eAlignment align,
                    const eChooseAction& action,
                    const eSkillChoice schoice);
};

#endif // ESKILLSELECTWIDGET_H
