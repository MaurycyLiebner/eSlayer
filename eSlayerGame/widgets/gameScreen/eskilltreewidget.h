#ifndef ESKILLTREEWIDGET_H
#define ESKILLTREEWIDGET_H

#include "../ewidget.h"

struct eStats;
struct eAttributes;
struct eEquipment;

class eSkillTreeWidget : public eWidget {
public:
    using eWidget::eWidget;

    void initialize(const int skillTreeId,
                    eStats& stats);
};

#endif // ESKILLTREEWIDGET_H
