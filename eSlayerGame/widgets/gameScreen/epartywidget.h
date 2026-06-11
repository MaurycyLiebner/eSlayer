#ifndef EPARTYWIDGET_H
#define EPARTYWIDGET_H

#include "ebgwidget.h"

#include <eSlayerHelpers/eteamid.h>

class ePartyWidget : public eBgWidget {
public:
    using eBgWidget::eBgWidget;

    void initialize(const std::string& name);
protected:
    void paintEvent(ePainter& p) override;
    bool mousePressEvent(const eMouseEvent& e) override;
private:
    void updatePartyWidgets();

    uint16_t mTeamsVersion = 0;
    eTeamId mClientTeam = eTeamId::playerTeam0;
    int mPlayerWidgetWidth = 0;
    int mPlayerWidgetHeight = 0;
    eWidget* mCenterWidget = nullptr;
};

#endif // EPARTYWIDGET_H
