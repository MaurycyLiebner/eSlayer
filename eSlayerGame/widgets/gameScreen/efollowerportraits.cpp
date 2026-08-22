#include "efollowerportraits.h"

#include "../../egameworld.h"
#include "../etexturecheckbutton.h"
#include "../../textures/euitextures.h"
#include "../../textures/etextgenerator.h"
#include "../ecolors.h"
#include "egamewidget.h"

#include <eSlayerHelpers/evectorhelpers.h>

class ePortrait : public eTextureCheckButton {
public:
    using eTextureCheckButton::eTextureCheckButton;

    void initialize(const eUnit& u, const std::string& name,
                    const bool stack) {
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

    void incCount() {
        mCount++;
    }

    void zeroCount() {
        mCount = 0;
    }

    int count() const { return mCount; }
protected:
    void paintEvent(ePainter& p) override {
        eTextureCheckButton::paintEvent(p);
        const int tw = width();
        const int th = height();
        const int w = mHealthFrac*tw;
        p.fillRect(SDL_Rect{0, th, w, th/8}, eColors::sHealth);
        if(mCount != mTexCount) {
            if(mCount <= 1) {
                mCountTex = nullptr;
            } else {
                const auto r = renderer();
                const auto& res = resolution();
                const int fs = res.smallFontSize();
                const auto font = eFonts::defaultFont(fs);
                eTextGenerator gen(r, eFontColor::white, font);
                mCountTex = gen.generate(std::to_string(mCount));
            }
            mTexCount = mCount;
        }
        if(mCountTex) {
            const auto rect = eWidget::rect();
            p.drawTexture(rect, mCountTex,
                          eAlignment::bottom | eAlignment::right);
        }
        if(!mName) return;
        p.drawTexture(tw/2, th + th/8, mName, eAlignment::hcenter);
    }
private:
    bool mStack = false;
    int mCount = 1;
    int mTexCount = 0;
    std::shared_ptr<eTexture> mCountTex;
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

bool eFollowerPortraits::dropItem() {
    for(const auto& it : mUnitPortraits) {
        const auto p = it.second;
        if(!p->hovered()) continue;
        const auto unitId = it.first;
        mDropAction(unitId);
        return true;
    }
    return false;
}

void eFollowerPortraits::paintEvent(
    ePainter& p) {
    updateFollowers();
}

void eFollowerPortraits::updateFollowers() {
    for(const auto p : mAllPortraits) {
        p->zeroCount();
    }

    std::vector<ePortrait*> allPortraits;

    std::set<uint32_t> presentUnits;
    std::set<uint8_t> presentStack;
    const auto handleUnit = [&](const uint32_t unitId,
                                const bool stack) {
        const auto u = mWorld->getUnit(unitId);
        if(!u) return;
        presentUnits.emplace(unitId);
        const auto infoId = u->fUnitInfoId;
        if(stack) presentStack.emplace(infoId);
        const auto& it = mUnitPortraits.find(unitId);
        if(it != mUnitPortraits.end()) {
            const auto p = it->second;
            p->update(*u);
            p->incCount();
            if(p->count() == 1) {
                allPortraits.emplace_back(p);
            }
            return;
        }
        if(stack) {
            const auto& it = mStackPortraits.find(infoId);
            if(it != mStackPortraits.end()) {
                const auto p = it->second;
                p->update(*u);
                p->incCount();
                if(p->count() == 1) {
                    allPortraits.emplace_back(p);
                }
                mUnitPortraits.emplace(unitId, p);
                return;
            }
        }
        const auto p = new ePortrait(window());
        const auto name = mGW->name(u);
        p->initialize(*u, name, stack);
        p->setCheckAction([this, unitId](const bool) {
            mPressAction(unitId);
        });
        allPortraits.emplace_back(p);
        mUnitPortraits.emplace(unitId, p);
        if(stack) {
            mStackPortraits.emplace(infoId, p);
        }
    };

    const auto& merc = mGW->merc();
    const auto mercId = merc ? merc->fUnitId : 0;

    if(merc) handleUnit(mercId, false);

    std::set<uint32_t> slayers;
    const auto clientId = mGW->clientId();
    const auto team = eTeams::playerTeam(clientId);
    for(const auto& it : eSlayers::sSlayers) {
        const auto unitId = it.first;
        if(unitId == clientId) continue;
        const auto& s = it.second;
        if(s.fTeamId != team) {
            continue;
        }
        slayers.emplace(unitId);
    }
    for(const auto unitId : slayers) {
        handleUnit(unitId, false);
    }

    std::set<uint32_t> followers;
    for(const auto& it : eFollowers::sFollowers) {
        const auto unitId = it.first;
        if(unitId == mercId) continue;
        const auto& f = it.second;
        if(f.fHealth <= 0) {
            continue;
        }
        followers.emplace(unitId);
    }
    for(const auto f : followers) {
        handleUnit(f, true);
    }

    if(mAllPortraits != allPortraits) {
        removeAllWidgets();

        for(const auto p : mAllPortraits) {
            if(!eVectorHelpers::contains(allPortraits, p)) {
                p->deleteLater();
            }
        }
        for(const auto p : allPortraits) {
            addWidget(p);
        }
        mAllPortraits = allPortraits;

        const auto& res = resolution();
        const int pp = res.largePadding();
        stackVertically(3*pp);
        fitContent();
    }

    for(auto it = mUnitPortraits.begin(); it != mUnitPortraits.end();) {
        const auto id = it->first;
        if(presentUnits.count(id) > 0) {
            it++;
            continue;
        }
        it = mUnitPortraits.erase(it);
    }

    for(auto it = mStackPortraits.begin(); it != mStackPortraits.end();) {
        const auto id = it->first;
        if(presentStack.count(id) > 0) {
            it++;
            continue;
        }
        it = mStackPortraits.erase(it);
    }
}