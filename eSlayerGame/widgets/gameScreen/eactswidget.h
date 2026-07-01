#ifndef EACTSWIDGET_H
#define EACTSWIDGET_H

#include "ebgwidget.h"

class eMainMenuButton;

struct eAct {
    uint8_t fActId;
    eMainMenuButton* fButton = nullptr;
    eWidget* fWidget = nullptr;

    bool operator==(const eAct& act) const {
        return fActId == act.fActId;
    }
};

class eActsWidget : public eBgWidget {
public:
    using eBgWidget::eBgWidget;
protected:
    void initialize(const std::string& title,
                    const std::vector<eAct>& acts,
                    const eAct& currentAct);
private:
    std::vector<eAct> mActs;
};

#endif // EACTSWIDGET_H
