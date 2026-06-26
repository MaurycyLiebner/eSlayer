#include "ebgwidget.h"

#include "../../textures/euitextures.h"

#include <cmath>

void eBgWidget::setup(eWidget* const inner) {
    const auto& res = resolution();
    const int innerP = 25*res.multiplier();
    addWidget(inner);
    resize(inner->width() + 2*innerP,
           inner->height() + 2*innerP);
    inner->align(eAlignment::center);
}

void eBgWidget::paintEvent(ePainter& p) {
    const auto& coll = eUITextures::sBg;
    const auto& first = coll.getTexture(0);
    const int dim = first->width();
    const int xMax = width()/dim;
    const int yMax = height()/dim;
    for(int x = 0; x <= xMax; x++) {
        for(int y = 0; y <= yMax; y++) {
            int id = 0;
            if(y == 0) {
                if(x == 0) {
                    id = 0;
                } else if(x == xMax) {
                    id = 2;
                } else {
                    id = 1;
                }
            } else if(y == yMax) {
                if(x == 0) {
                    id = 6;
                } else if(x == xMax) {
                    id = 8;
                } else {
                    id = 7;
                }
            } else {
                if(x == 0) {
                    id = 3;
                } else if(x == xMax) {
                    id = 5;
                } else {
                    id = 4;
                }
            }
            const auto& tex = coll.getTexture(id);
            int px = x*dim;
            if(x == xMax) px = width() - dim;
            int py = y*dim;
            if(y == yMax) py = height() - dim;
            p.drawTexture(px, py, tex);
        }
    }
}

bool eBgWidget::mousePressEvent(const eMouseEvent& e) {
    return true;
}