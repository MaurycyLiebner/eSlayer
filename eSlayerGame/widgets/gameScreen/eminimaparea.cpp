#include "eminimaparea.h"

#include "../epainter.h"

#include <eSlayerMapGenerator/emap.h>

#include <eSlayerHelpers/eobject.h>
#include <eSlayerHelpers/eobjectsinfo.h>

eMiniMapArea::eMiniMapArea(
    const eResolution* res,
    SDL_Renderer* const r) :
    fRes(res), fR(r) {}

void eMiniMapArea::initialize() {
    if(fInitialized) return;
    fInitialized = true;

    fKnown.resize(sAreaDim, std::vector<bool>(sAreaDim, false));

    const auto& tex = eMapTextures::sWalls.getTexture(0);
    fTileW = tex->width();
    fTileH = tex->height();

    fTex = std::make_shared<eTexture>();
    const int w = sAreaDim * fTileW;
    const int h = sAreaDim * fTileH;
    fTex->create(fR, w, h);
}

void eMiniMapArea::setKnown(
    ePointF pos,
    const int x0, const int y0,
    eMap& map) {
    if(fUnknown <= 0) return;
    pos.fX -= x0;
    pos.fY -= y0;
    initialize();
    const auto h = fTex->createTargetHolder(fR);
    ePainter p(fR);

    const int margin = 9;

    const int dx0 = pos.fX - margin;
    const int dx1 = pos.fX + margin;

    const int dy0 = pos.fY - margin;
    const int dy1 = pos.fY + margin;

    const float xOffset = fTex->width() / 2.f;
    for(int x = dx0; x <= dx1; x++) {
        if(x < 0) x = 0;
        else if(x >= sAreaDim) break;
        for(int y = dy0; y <= dy1; y++) {
            if(y < 0) y = 0;
            else if(y >= sAreaDim) break;
            const ePointF xypos{x, y};
            const float dist = ePointF::distance(pos, xypos);
            if(dist > margin) continue;
            const bool known = fKnown[y][x];
            if(known) continue;
            setKnown(p, x0, y0, x, y, map, xOffset);
        }
    }
}

bool eMiniMapArea::setKnown(
    ePainter& p,
    const int x0, const int y0,
    const int x, const int y,
    eMap& map, const float xOffset) {
    const int mx = x + x0;
    const int my = y + y0;
    const bool has = map.hasPortion(mx, my);
    if(!has) return false;
    fKnown[y][x] = true;
    fUnknown--;
    const bool w = !map.hasObjects(mx, my) &&
                   map.walkable(ePointF(mx, my));
    if(w) return true;
    bool walkable[3][3];
    for(int dx = -1; dx <= 1; dx++) {
        for(int dy = -1; dy <= 1; dy++) {
            const int mx2 = mx + dx;
            const int my2 = my + dy;
            const bool w = !map.hasObjects(mx2, my2) &&
                           map.walkable(ePointF(mx2, my2));
            walkable[1 + dy][1 + dx] = w;
        }
    }
    const float texX = xOffset + (x - y) * (fTileW / 2.0f);
    const float texY = (x + y) * (fTileH / 2.0f);

    int texId1 = -1;
    int texId2 = -1;

    int mask = 0;

    if(!walkable[0][1]) mask |= 1; // top-right
    if(!walkable[1][2]) mask |= 2; // bottom-right
    if(!walkable[2][1]) mask |= 4; // bottom-left
    if(!walkable[1][0]) mask |= 8; // top-left

    if(mask == 0) {
        texId1 = 12; // no surrounding walls (fully open)
    } else if(mask == 1) {
        texId1 = 8; // wall only top-right
    } else if(mask == 2) {
        texId1 = 9; // wall only bottom-right
    } else if(mask == 3) {
        texId1 = 3; // walls top-right + bottom-right (vertical edge on right)
    } else if(mask == 4) {
        texId1 = 10; // wall only bottom-left
    } else if(mask == 5) {
        texId1 = 1; // walls top-right + bottom-left (diagonal split)
        texId2 = 7;
    } else if(mask == 6) {
        texId1 = 4; // walls bottom-right + bottom-left (horizontal edge bottom)
    } else if(mask == 7) {
        texId1 = 7; // walls top-right + bottom-right + bottom-left (missing top-left)
    } else if(mask == 8) {
        texId1 = 11; // wall only top-left
    } else if(mask == 9) {
        texId1 = 2; // walls top-left + top-right (horizontal edge top)
    } else if(mask == 10) {
        texId1 = 0; // walls top-left + bottom-right (diagonal split)
        texId2 = 6;
    } else if(mask == 11) {
        texId1 = 0; // walls top-left + top-right + bottom-right (missing bottom-left)
    } else if(mask == 12) {
        texId1 = 5; // walls top-left + bottom-left (vertical edge on left)
    } else if(mask == 13) {
        texId1 = 1; // walls top-left + top-right + bottom-left (missing bottom-right)
    } else if(mask == 14) {
        texId1 = 6; // walls bottom-left + bottom-right + top-left (missing top-right) → reuse
    } else if(mask == 15) {
        if(walkable[0][0]) {
            texId1 = 13;
        } else if(walkable[0][2]) {
            texId1 = 14;
        } else if(walkable[2][2]) {
            texId1 = 15;
        } else if(walkable[2][0]) {
            texId1 = 16;
        } else {
            return true;
        }
    }

    {
        const auto& tile = map.tile(mx, my);
        bool cross = false;
        int crossX = texX;
        int crossY = texY;
        if(tile.fStairsTL) {
            cross = true;
            crossX -= fTileW/4;
            crossY += fTileH/4;
        } else if(tile.fStairsTR) {
            cross = true;
            crossX += fTileW/4;
            crossY += fTileH/4;
        } else {
            const auto& objIds = map.objects(mx, my);
            for(const auto id : objIds) {
                const auto& o = map.object(id);
                const auto objectType = o->fObjectType;
                const auto& info = eObjectsInfo::sObjects.get(objectType);
                switch(info.fType) {
                case eObjectType::trapDoor:
                case eObjectType::portalDoor: {
                    cross = true;
                    const auto& pos = o->fPos;
                    const float dx = pos.fX - mx;
                    const float dy = pos.fY - my;
                    crossX += (dx - dy)*fTileW/2;
                    crossY += (dx + dy)*fTileH/2;
                } break;
                default:
                    break;
                };
                if(cross) break;
            }
        }
        if(cross) {
            const int dim = fTileH/2;
            const int thick = std::max(1, dim/2);
            p.drawCross(crossX, crossY, dim, thick,
                        SDL_Color{0, 0, 255, 255});
        }
    }

    if(texId1 != -1) {
        const auto& tex1 = eMapTextures::sWalls.getTexture(texId1);
        p.drawTexture(texX, texY, tex1, eAlignment::hcenter);
    }
    if(texId2 != -1) {
        const auto& tex2 = eMapTextures::sWalls.getTexture(texId2);
        p.drawTexture(texX, texY, tex2, eAlignment::hcenter);
    }
    return true;
}

bool eMiniMapArea::tryFillingPastKnown(
    const int x0, const int y0, eMap& map) {
    if(fPastKnown.empty()) return true;
    initialize();
    const auto h = fTex->createTargetHolder(fR);
    ePainter p(fR);
    const float xOffset = fTex->width() / 2.f;
    for(int x = 0; x < sAreaDim; x++) {
        for(int y = 0; y < sAreaDim; y++) {
            const bool k = fPastKnown[y][x];
            if(!k) continue;
            const bool r = setKnown(p, x0, y0, x, y, map, xOffset);
            if(!r) return false;
            fPastKnown[y][x] = false;
        }
    }
    fPastKnown.clear();
    return true;
}
