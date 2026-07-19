#ifndef ESKILLTREEWIDGET_H
#define ESKILLTREEWIDGET_H

#include "../ewidget.h"

struct eStats;
struct eAttributes;
struct eEquipment;
class eSkillButton;

class eSkillTreeWidget : public eWidget {
public:
    using eWidget::eWidget;

    void initialize(const int skillTreeId,
                    eStats& stats);
protected:
    void paintEvent(ePainter& p) override;
private:
    std::vector<std::pair<eSkillButton*, eSkillButton*>> mPrerequisites;
};

#endif // ESKILLTREEWIDGET_H
