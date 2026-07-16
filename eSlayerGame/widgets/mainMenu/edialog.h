#ifndef EDIALOG_H
#define EDIALOG_H

#include "../elabel.h"

class eDialog : public eLabel {
public:
    using eLabel::eLabel;
protected:
    void paintEvent(ePainter& p) override;
};

#endif // EDIALOG_H
