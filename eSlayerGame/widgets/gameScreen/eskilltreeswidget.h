#ifndef ESKILLTREESWIDGET_H
#define ESKILLTREESWIDGET_H

#include "ebgwidget.h"

struct eStats;
struct eAttributes;
struct eEquipment;
class eSkillTreeWidget;
class eLabel;

class eSkillTreesWidget : public eBgWidget {
public:
    using eBgWidget::eBgWidget;

    void initialize(eStats& stats,
                    const eAttributes& attrs,
                    const eEquipment& eq);
protected:
    void paintEvent(ePainter& p) override;
private:
    eStats* mStats = nullptr;

    eLabel* mRemPtsLabel = nullptr;
    std::vector<eSkillTreeWidget*> mWidgets;
};

#endif // ESKILLTREESWIDGET_H
