#ifndef ESTATUSWIDGET_H
#define ESTATUSWIDGET_H

#include "../ewidget.h"

class eLabel;

struct eGameSettings {
    std::string fType;
    std::string fIP;
    std::string fPassword;
};

class eStatusWidget : public eWidget {
public:
    using eWidget::eWidget;

    void initialize(
        const eGameSettings& settings);

    void setAreaName(
        const std::string& name);
private:
    eLabel* mAreaName = nullptr;
};

#endif // ESTATUSWIDGET_H
