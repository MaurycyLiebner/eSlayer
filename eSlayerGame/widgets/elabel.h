#ifndef ELABEL_H
#define ELABEL_H

#include "ewidget.h"
#include "elabelbase.h"

class eLabel : public eWidget, public eLabelBase {
public:
    eLabel(eMainWindow* const window);
    eLabel(const std::string& text,
           eMainWindow* const window);

    void fitOptions(const std::vector<std::string>& options);
    void renderTargetsReset() override;
protected:
    void sizeHint(int& w, int& h) override;
    void paintEvent(ePainter& p) override;
};

#endif // ELABEL_H
