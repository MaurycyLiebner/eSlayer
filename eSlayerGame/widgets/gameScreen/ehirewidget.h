#ifndef EHIREWIDGET_H
#define EHIREWIDGET_H

#include "ebgwidget.h"

#include <eSlayerHelpers/ehireinfo.h>

class eHireWidget : public eBgWidget {
public:
    using eBgWidget::eBgWidget;

    using eHireAction = std::function<void(const eHireInfo&)>;
    void initialize(const std::vector<eHireInfo>& info,
                    const uint32_t gold,
                    const eHireAction& hireAction,
                    const eAction& closeAction);
};

#endif // EHIREWIDGET_H
