#ifndef EUNITINDICATOR_H
#define EUNITINDICATOR_H

#include "../eprogressbar.h"

class eUnit;

class eUnitIndicator : public eProgressBar {
public:
    using eProgressBar::eProgressBar;

    void initialize();

    void setUnit(const std::shared_ptr<eUnit>& u);
protected:
    void paintEvent(ePainter& p) override;
private:
    std::shared_ptr<eUnit> mUnit;
    eLabel* mName = nullptr;
};

#endif // EUNITINDICATOR_H
