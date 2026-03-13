#ifndef EHEALTHINDICATOR_H
#define EHEALTHINDICATOR_H

#include "../eprogressbar.h"

class eHealthIndicator : public eProgressBar {
public:
    using eProgressBar::eProgressBar;

    void initialize();
    void setColor(const SDL_Color& col);
    void showText();
    void hideText();
protected:
    void paintEvent(ePainter& p) override;
    void setText(const std::string& text);
private:
    eLabel* mText = nullptr;
    SDL_Color mColor;
};

#endif // EHEALTHINDICATOR_H
