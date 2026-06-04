#include "eopengenerator.h"

void eOpenGenerator::generate(
    const eRect& rect,
    std::vector<eChamber>& chambers,
    std::vector<eRect>& doors,
    const int margin) {
    const auto in = rect.inset(margin);
    const int dim = 10;
    for(int x = in.fX; x < in.fX + in.fW; x += dim) {
        for(int y = in.fY; y < in.fY + in.fH; y += dim) {
            const int w = std::min(dim, in.fX + in.fW - x);
            const int h = std::min(dim, in.fY + in.fH - y);
            const eRect rect{x, y, w, h};
            chambers.emplace_back(rect);
        }
    }
}
