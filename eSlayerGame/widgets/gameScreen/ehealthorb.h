#ifndef EHEALTHORB_H
#define EHEALTHORB_H

#include "../elabel.h"

class eHealthOrb : public eLabel {
public:
    using eLabel::eLabel;

    void initialize();

    void setColor(const SDL_Color& color);
    void setMax(const int max);
    void setValue(const int v);
protected:
    void paintEvent(ePainter& p) override;
private:
    SDL_Color mColor{255, 0, 0, 255};
    int mMax = 100;
    int mValue = 50;
};

#endif // EHEALTHORB_H
