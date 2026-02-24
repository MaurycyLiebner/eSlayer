#include "eserverarea.h"

void eServerArea::increment() {
    if(mUnits.empty()) {
        for(int x = 12; x < 24; x++) {
            for(int y = 12; y < 24; y++) {
                const auto u = std::make_shared<eServerUnit>();
                u->fCharId = eServerUnit::sNextCharId++;
                const ePointF pos{double(x), double(y)};
                u->fPos = pos;
                mUnits.emplace_back(u);

                const auto m = std::make_shared<eMovementHandler>();
                m->setPos(pos);
                mMovementHandlers.emplace_back(m);
            }
        }
    }

    const int iMax = mMovementHandlers.size();
    for(int i = 0; i < iMax; i++) {
        const auto& m = mMovementHandlers[i];
        if(!m) continue;
        m->increment();
        const auto& u = mUnits[i];
        u->fPos = m->pos();
    }
}
