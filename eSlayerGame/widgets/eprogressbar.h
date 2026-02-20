#ifndef EPROGRESSBAR_H
#define EPROGRESSBAR_H

#include "elabel.h"

class eProgressBar : public eLabel {
public:
    using eLabel::eLabel;

    int value() const { return mValue; }

    void setRange(const int min, const int max);
    void setValue(const int v);
protected:
    void paintEvent(ePainter& p);
private:
    int mMin = 0;
    int mMax = 100;
    int mValue = 0;
};

#endif // EPROGRESSBAR_H
