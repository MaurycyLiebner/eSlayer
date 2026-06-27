#include "eminimap.h"

#include "../../textures/etextgenerator.h"
#include "estatuswidget.h"

#include <eSlayerHelpers/erunsettings.h>
#include <eSlayerHelpers/eslayers.h>
#include <eSlayerHelpers/eteamid.h>
#include <eSlayerHelpers/ebody.h>
#include <eSlayerHelpers/eportals.h>

#include <eSlayerMapGenerator/emapgenerator.h>

std::map<std::string, eMiniMap::eAreas> eMiniMap::sAreasMap;
eMiniMap* eMiniMap::sInstance = nullptr;
bool eMiniMap::sShowMap = false;

eMiniMap::eMiniMap(eMainWindow* const window) :
    eWidget(window) {
    sInstance = this;

    const auto& tex = eMapTextures::sWalls.getTexture(0);
    fTileW = tex->width();
    fTileH = tex->height();
}

eMiniMap::~eMiniMap() {
    for(auto& row : mAreas) {
        for(auto& a : row) {
            if(!a.fInitialized) continue;
            a.fTex = nullptr;
            const int dim = eMiniMapArea::sAreaDim;
            if(a.fPastKnown.empty()) {
                a.fPastKnown.resize(dim, std::vector<bool>(dim, false));
            }
            for(int y = 0; y < dim; y++) {
                for(int x = 0; x < dim; x++) {
                    a.fPastKnown[y][x] = a.fPastKnown[y][x] || a.fKnown[y][x];
                }
            }
            a.fKnown.clear();
            a.fInitialized = false;
        }
    }
    sAreasMap[mMapName] = mAreas;
    sInstance = nullptr;
}

void eMiniMap::clearAll() {
    sAreasMap.clear();
}

void eMiniMap::initialize(const eGameSettings& settings) {
    mStatusWidget = new eStatusWidget(window());
    mStatusWidget->initialize(settings);
    addWidget(mStatusWidget);
    mStatusWidget->align(eAlignment::top | eAlignment::right);
    const auto& res = resolution();
    const float mult = res.multiplier();
    const int x = mStatusWidget->x();
    const int y = mStatusWidget->y();
    const int margin = mult*25;
    mStatusWidget->move(x - margin, y + margin);
    setShowMap(sShowMap);
}

void eMiniMap::setMap(const std::shared_ptr<eMap>& map) {
    mMap = map;
    const auto mapId = map->id();
    mMapName = eMapsSettings::sMaps.name(mapId);

    const auto it = sAreasMap.find(mMapName);
    if(it == sAreasMap.end()) {
        const int areaDim = eMiniMapArea::sAreaDim;
        const int w = (mMap->width() + areaDim - 1)/areaDim;
        const int h = (mMap->height() + areaDim - 1)/areaDim;

        const auto r = renderer();
        const auto& res = resolution();
        mAreas.resize(h, std::vector<eMiniMapArea>(w, {&res, r}));
    } else {
        mAreas = it->second;
    }
}

void eMiniMap::setPos(const ePointF& pos) {
    mPos = pos;
}

void eMiniMap::sShowAreaName(const std::string& name) {
    if(!sInstance) return;
    sInstance->showAreaName(name);
}

void eMiniMap::paintEvent(ePainter& p) {
    if(mMap) {
        const int areaDim = eMiniMapArea::sAreaDim;
        const int w = (mMap->width() + areaDim - 1)/areaDim;
        const int h = (mMap->height() + areaDim - 1)/areaDim;

        const int cx = mPos.fX;
        const int cy = mPos.fY;

        const int margin = 8;

        const int x0 = std::clamp(cx/areaDim - margin, 0, w - 1);
        const int x1 = std::clamp(cx/areaDim + margin, 0, w - 1);

        const int y0 = std::clamp(cy/areaDim - margin, 0, h - 1);
        const int y1 = std::clamp(cy/areaDim + margin, 0, h - 1);

        for(int x = x0; x <= x1; x++) {
            for(int y = y0; y <= y1; y++) {
                const ePointF pos{float(x*areaDim),
                                  float(y*areaDim)};
                const float dist = ePointF::distance(pos, mPos);
                auto& a = mAreas[y][x];
                const int x0 = x*areaDim;
                const int y0 = y*areaDim;
                a.tryFillingPastKnown(x0, y0, *mMap);
                if(dist < 20.f) {
                    a.setKnown(mPos, x0, y0, *mMap);
                }
                const auto& tex = a.fTex;
                if(!tex) continue;
                const float dx = x*areaDim - mPos.fX;
                const float dy = y*areaDim - mPos.fY;
                const int texW = tex->width();
                const int texH = tex->height();
                const int xx = mHPos*width() - texW/2.f + (dx - dy) * (a.fTileW / 2.f);
                const int yy = 0.5f*height() - texH/2.f + (dx + dy) * (a.fTileH / 2.f);
                if(mShowMap) p.drawTexture(xx, yy, tex);
            }
        }

        if(mShowMap) {
            const auto drawCross = [&](const ePointF& pos,
                                       const SDL_Color& color,
                                       const uint32_t clientId) {
                const float dx = pos.fX - mPos.fX;
                const float dy = pos.fY - mPos.fY;
                const int xx = mHPos*width() + (dx - dy) * (fTileW / 2.f);
                const int yy = 0.5f*height() + (dx + dy) * (fTileH / 2.f) - 4*fTileH;
                const int dim = fTileH/2;
                const int thick = std::max(1, dim/2);
                p.drawCross(xx, yy, dim, thick, color);
                if(!clientId) return;
                if(clientId != eSlayers::sThisSlayer) {
                    const auto tex = requestNameTex(clientId);
                    p.drawTexture(xx, yy - 2*dim, tex,
                                  eAlignment::top | eAlignment::hcenter);
                }
            };

            const auto mapId = mMap->id();
            for(const auto& s : eSlayers::sSlayers) {
                const auto clientId = s.first;
                const auto& slayer = s.second;
                if(slayer.fMapId != mapId) continue;
                const auto t1 = eTeams::playerTeam(clientId);
                const auto t2 = eTeams::playerTeam(eSlayers::sThisSlayer);
                if(t1 != t2) continue;
                const auto& pos = clientId == eSlayers::sThisSlayer ?
                    mPos : slayer.fPos;
                drawCross(pos, SDL_Color{0, 255, 0, 255},
                          clientId);
            }
            for(const auto& b : eBodies::sBodies) {
                if(b.fMapId != mapId) continue;
                drawCross(b.fPos, SDL_Color{255, 0, 0, 255},
                          eSlayers::sThisSlayer);
            }
            for(const auto& p : ePortal::sPortals) {
                const auto& ca = p.fCampArea;
                if(ca.fMapId == mapId) {
                    drawCross(p.fCampPos, SDL_Color{0, 0, 255, 255}, 0);
                }
                const auto& oa = p.fOutdoorArea;
                if(oa.fMapId == mapId) {
                    drawCross(p.fOutdoorPos, SDL_Color{0, 0, 255, 255}, 0);
                }
            }
        }
    }

    if(mAreaStrCounter-- > 0 && mAreaStr) {
        const auto& rect = eWidget::rect();
        p.drawTexture(rect, mAreaStr, eAlignment::center);
    } else {
        mAreaStr = nullptr;
    }
}

void eMiniMap::showAreaName(const std::string& name) {
    const auto r = renderer();
    const auto& res = resolution();
    const int fs = res.extraHugeFontSize();
    const auto font = eFonts::defaultFont(fs);
    const int shift = res.lineWidth();
    const eTextGenerator gen(r, eFontColor::redBlack, font, shift);
    mAreaStr = gen.generate(name);
    mAreaStrCounter = 6*eRunSettings::sFPS;

    mStatusWidget->setAreaName(name);
}

std::shared_ptr<eTexture>
eMiniMap::requestNameTex(
    const uint32_t clientId) {
    {
        const auto it = mNameTexs.find(clientId);
        if(it != mNameTexs.end()) {
            return it->second;
        }
    }
    const auto it = eSlayers::sSlayers.find(clientId);
    if(it == eSlayers::sSlayers.end()) return nullptr;
    const auto& slayer = it->second;
    const auto& name = slayer.fName;
    const auto r = renderer();
    const auto& res = resolution();
    const int fs = res.smallFontSize();
    const auto font = eFonts::defaultFont(fs);
    const eTextGenerator gen(r, eFontColor::green, font);
    const auto tex = gen.generate(name);
    mNameTexs[clientId] = tex;
    return tex;
}

void eMiniMap::setShowMap(const bool s) {
    mShowMap = s;
    sShowMap = s;
    mStatusWidget->setVisible(s);
}

void eMiniMap::switchShowMap() {
    setShowMap(!mShowMap);
}
