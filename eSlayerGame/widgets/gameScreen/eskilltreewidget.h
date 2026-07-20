#ifndef ESKILLTREEWIDGET_H
#define ESKILLTREEWIDGET_H

#include "../ewidget.h"

struct eStats;
struct eAttributes;
struct eEquipment;
class eSkillTreeButton;

class eSkillTreeWidget : public eWidget {
public:
    using eWidget::eWidget;

    void initialize(const int skillTreeId,
                    eStats& stats);
protected:
    void paintEvent(ePainter& p) override;
private:
    void updateEnabled();

    const eStats* mStats = nullptr;
    std::vector<eSkillTreeButton*> mButtons;
    using eButtonPair = std::pair<eSkillTreeButton*, eSkillTreeButton*>;
    std::vector<eButtonPair> mPrerequisites;
};

#endif // ESKILLTREEWIDGET_H
