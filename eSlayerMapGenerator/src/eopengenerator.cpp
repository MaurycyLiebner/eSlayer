#include "eopengenerator.h"

void eOpenGenerator::generate(
    const eRect& rect,
    std::vector<eChamber>& chambers,
    std::vector<eRect>& doors,
    const int margin) {
    const auto in = rect.inset(margin);
    chambers.emplace_back(in);
}
