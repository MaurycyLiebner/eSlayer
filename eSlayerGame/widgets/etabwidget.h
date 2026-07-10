#ifndef ETABWIDGET_H
#define ETABWIDGET_H

#include "ewidget.h"

class eTabWidget : public eWidget {
public:
    using eWidget::eWidget;

    void initialize();

    void addTab(const std::string& name,
                eWidget* const widget);

    void showTab(const int tabId);
private:
    eWidget* mTabs = nullptr;
    eWidget* mWidget = nullptr;
    std::vector<eWidget*> mWidgets;
};

#endif // ETABWIDGET_H
