#include "efollowerportraits.h"

#include "../../egameworld.h"
#include "../etexturecheckbutton.h"
#include "../../textures/euitextures.h"
#include "../ecolors.h"

class ePortrait : public eTextureCheckButton {
public:
    using eTextureCheckButton::eTextureCheckButton;

    void initialize(const eUnit& u) {
        const auto& ps = eUITextures::sPortraits;
        const auto it = ps.find(u.fUnitInfoId);
        if(it == ps.end()) return;
        const auto& p = it->second;
        eTextureCheckButton::initialize(p, p);
        update(u);
    }

    void update(const eUnit& u) {
        mHealthFrac = float(u.fHealth)/u.fMaxHealth;
    }
protected:
    void paintEvent(ePainter& p) override {
        eTextureCheckButton::paintEvent(p);
        const int tw = width();
        const int th = height();
        const int w = mHealthFrac*tw;
        p.fillRect(SDL_Rect{0, th, w, th/8}, eColors::sHealth);
    }
private:
    float mHealthFrac = 1.f;
};

void eFollowerPortraits::initialize(
    const eGameWorld& world,
    const int w, const int h,
    const ePressAction& pressA,
    const eDropAction& dropA) {
    resize(w, h);
    mWorld = &world;
    mPressAction = pressA;
    mDropAction = dropA;
}

void eFollowerPortraits::addFollower(
    const uint32_t follower) {
    mFollowers.emplace(follower);
}

bool eFollowerPortraits::dropItem() {
    for(const auto& it : mPortraits) {
        const auto p = it.second;
        if(!p->hovered()) continue;
        const auto unitId = it.first;
        const bool r = mDropAction(unitId);
        if(r) return true;
    }
    return false;
}

void eFollowerPortraits::paintEvent(
    ePainter& p) {
    updateFollowers();
}

void eFollowerPortraits::updateFollowers() {
    const auto followers = mFollowers;
    for(const auto f : followers) {
        const auto u = mWorld->getUnit(f);
        if(u) {
            updatePortrait(*u);
        } else {
            mFollowers.erase(f);
            removePortrait(f);
        }
    }
}

void eFollowerPortraits::updatePortrait(const eUnit& u) {
    const auto it = mPortraits.find(u.fCharId);
    if(it == mPortraits.end()) {
        addPortrait(u);
        return;
    }
    const auto p = it->second;
    p->update(u);
}

void eFollowerPortraits::addPortrait(const eUnit& u) {
    const auto p = new ePortrait(window());
    p->initialize(u);
    const auto id = u.fCharId;
    p->setCheckAction([this, id](const bool) {
        mPressAction(id);
    });
    addWidget(p);
    const auto& res = resolution();
    const int pp = res.largePadding();
    stackHorizontally(pp);
    mPortraits[u.fCharId] = p;
}

void eFollowerPortraits::removePortrait(const uint32_t uid) {
    const auto it = mPortraits.find(uid);
    if(it == mPortraits.end()) return;
    const auto p = it->second;
    p->deleteLater();
    mPortraits.erase(it);
}
