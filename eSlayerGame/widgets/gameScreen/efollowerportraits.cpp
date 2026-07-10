#include "efollowerportraits.h"

#include "../../egameworld.h"
#include "../etexturecheckbutton.h"
#include "../../textures/euitextures.h"
#include "../../textures/etextgenerator.h"
#include "../ecolors.h"
#include "egamewidget.h"

class ePortrait : public eTextureCheckButton {
public:
    using eTextureCheckButton::eTextureCheckButton;

    void initialize(const eUnit& u, const std::string& name) {
        const auto& ps = eUITextures::sPortraits;
        const auto it = ps.find(u.fUnitInfoId);
        if(it == ps.end()) return;
        const auto& p = it->second;
        eTextureCheckButton::initialize(p, p);
        update(u);

        const auto r = renderer();
        const auto& res = resolution();
        const int fs = res.tinyFontSize();
        const auto font = eFonts::defaultFont(fs);
        eTextGenerator gen(r, eFontColor::white, font);
        mName = gen.generate(name);
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
        if(!mName) return;
        p.drawTexture(tw/2, th + th/8, mName, eAlignment::hcenter);
    }
private:
    float mHealthFrac = 1.f;
    std::shared_ptr<eTexture> mName;
};

void eFollowerPortraits::initialize(
    const eGameWidget& gw,
    const eGameWorld& world,
    const ePressAction& pressA,
    const eDropAction& dropA) {
    mGW = &gw;
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
    const auto handleUnit = [&](const uint32_t unitId) {
        const auto u = mWorld->getUnit(unitId);
        if(u) {
            updatePortrait(*u);
        } else {
            removePortrait(unitId);
        }
    };
    for(const auto f : followers) {
        handleUnit(f);
    }
    const auto clientId = mGW->clientId();
    const auto team = eTeams::playerTeam(clientId);
    for(const auto& it : eSlayers::sSlayers) {
        const auto unitId = it.first;
        if(unitId == clientId) continue;
        const auto& s = it.second;
        if(s.fTeamId != team) {
            removePortrait(unitId);
            continue;
        }
        mSlayers.emplace(unitId);
    }
    const auto slayers = mSlayers;
    for(const auto unitId : slayers) {
        handleUnit(unitId);
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
    const auto name = mGW->name(u);
    p->initialize(u, name);
    const auto id = u.fCharId;
    p->setCheckAction([this, id](const bool) {
        mPressAction(id);
    });
    addWidget(p);
    afterChanged();
    mPortraits[u.fCharId] = p;
}

void eFollowerPortraits::removePortrait(const uint32_t uid) {
    mFollowers.erase(uid);
    mSlayers.erase(uid);
    const auto it = mPortraits.find(uid);
    if(it == mPortraits.end()) return;
    const auto p = it->second;
    p->deleteLater();
    afterChanged();
    mPortraits.erase(it);
}

void eFollowerPortraits::afterChanged() {
    const auto& res = resolution();
    const int pp = res.largePadding();
    stackVertically(3*pp);
    fitContent();
}
