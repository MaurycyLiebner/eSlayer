#include "etabwidget.h"

#include "ebuttonbase.h"

void eTabWidget::initialize() {
    setNoPadding();
    mTabs = new eWidget(window());
    mTabs->setNoPadding();
    addWidget(mTabs);
    mWidget = new eWidget(window());
    mWidget->setNoPadding();
    addWidget(mWidget);
}

void eTabWidget::addTab(
    const std::string& name,
    eWidget* const widget) {
    const auto tabButton = new eButtonBase(window());
    tabButton->setNoPadding();
    tabButton->setText(name);
    tabButton->fitContent();
    tabButton->setMouseEnterAction([tabButton]() {
        tabButton->setFontColor(eFontColor::blue);
    });
    tabButton->setMouseLeaveAction([tabButton]() {
        tabButton->setFontColor(eFontColor::white);
    });
    const int tabId = mWidgets.size();
    tabButton->setPressAction([this, tabId]() {
        showTab(tabId);
    });

    mTabs->addWidget(tabButton);
    const auto& res = resolution();
    const int p = res.largePadding();
    mTabs->stackHorizontally(p);
    mTabs->fitContent();

    mWidget->addWidget(widget);
    mWidget->fitContent();
    mWidgets.emplace_back(widget);
    for(const auto w : mWidgets) {
        w->align(eAlignment::center);
    }

    stackVertically(p);
    fitContent();

    showTab(tabId);
}

void eTabWidget::showTab(const int tabId) {
    for(const auto w : mWidgets) {
        w->hide();
    }
    mWidgets[tabId]->show();
}