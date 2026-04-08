#ifndef ESKILLTREESWIDGET_H
#define ESKILLTREESWIDGET_H

#include "../ewidget.h"

struct eStats;
struct eAttributes;
struct eEquipment;
class eSkillTreeWidget;
class eLabel;

class eSkillTreesWidget : public eWidget {
public:
    using eWidget::eWidget;

    void initialize(eStats& stats,
                    const eAttributes& attrs,
                    const eEquipment& eq);
protected:
    void paintEvent(ePainter& p) override;
    bool mousePressEvent(const eMouseEvent& e) override;
private:
    eStats* mStats = nullptr;

    eLabel* mRemPtsLabel = nullptr;
    std::vector<eSkillTreeWidget*> mWidgets;
};

#endif // ESKILLTREESWIDGET_H
