#ifndef ECOINSQUESTIONWIDGET_H
#define ECOINSQUESTIONWIDGET_H

#include "ebgwidget.h"

using eGoldAction = std::function<void(const int count)>;

class eCoinsQuestionWidget : public eBgWidget {
public:
    using eBgWidget::eBgWidget;
    ~eCoinsQuestionWidget();

    void initialize(const eGoldAction& a,
                    const int s, const int max);
protected:
    bool mouseMoveEvent(const eMouseEvent& e) override;
};

#endif // ECOINSQUESTIONWIDGET_H
