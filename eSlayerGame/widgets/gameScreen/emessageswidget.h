#ifndef EMESSAGESWIDGET_H
#define EMESSAGESWIDGET_H

#include "ebgwidget.h"

class eMessagesWidget : public eBgWidget {
public:
    using eBgWidget::eBgWidget;

    void initialize(const std::vector<std::string>& log);
protected:
    void paintEvent(ePainter& p) override;
private:
    void updateText();

    int mLatestCount = 0;
    const std::vector<std::string>* mLog = nullptr;
    eWidget* mContainer = nullptr;
};

#endif // EMESSAGESWIDGET_H
