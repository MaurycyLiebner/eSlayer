#include "edialog.h"

void eDialog::paintEvent(ePainter& p) {
    const auto rect = eWidget::rect();
    p.fillRect(rect, SDL_Color{0, 0, 0, 255});
    const int lineWidth = eLabel::lineWidth();
    p.drawRect(rect, SDL_Color{255, 255, 255, 255}, lineWidth);
}
