#ifndef EMINIMAP_H
#define EMINIMAP_H

#include "../../textures/emaptextures.h"
#include "../ewidget.h"

#include <eSlayerHelpers/epoint.h>
#include <eSlayerMapGenerator/emapgenerator.h>

class eMiniMap : public eWidget {
public:
    eMiniMap(eMainWindow* const window);
    ~eMiniMap();

    void setMap(const std::shared_ptr<eMap>& map);
    void setPos(const ePointF& pos);
    void setShowMap(const bool s)
    { mShowMap = s; }
    void switchShowMap()
    { mShowMap = !mShowMap; }

    static void sShowAreaName(const std::string& name);
protected:
    void paintEvent(ePainter& p) override;
private:
    static eMiniMap* sInstance;

    void showAreaName(const std::string& name);

    std::shared_ptr<eMap> mMap;
    ePointF mPos;

    struct eMiniMapArea {
        static const int sAreaDim = 10;
        eMiniMapArea(const eResolution* res,
                     SDL_Renderer* const r) :
            fRes(res), fR(r) {}

        void initialize() {
            if(fInitialized) return;
            fInitialized = true;

            fKnown.resize(sAreaDim, std::vector<bool>(sAreaDim, false));

            const int id = eMapTextures::sTexs.id("tile");
            fTileTex = eMapTextures::sTexs.get(id);

            fTileW = fTileTex->width();
            fTileH = fTileTex->height();
            const int w = sAreaDim*fTileW;
            const int h = sAreaDim*fTileH;

            fTex = std::make_shared<eTexture>();
            fTex->create(fR, w, h);
            fTex->setAlpha(128);
        }

        void setKnown(ePointF pos,
                      const int x0, const int y0,
                      const eMap& map) {
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

            const float xOffset = fTex->width()/2.f;
            for(int x = dx0; x <= dx1; x++) {
                if(x < 0) x = 0;
                else if(x >= sAreaDim) break;
                for(int y = dy0; y <= dy1; y++) {
                    if(y < 0) y = 0;
                    else if(y >= sAreaDim) break;
                    const ePointF xypos{float(x), float(y)};
                    const float dist = ePointF::distance(pos, xypos);
                    if(dist > margin) continue;
                    const bool known = fKnown[y][x];
                    if(known) continue;
                    fKnown[y][x] = true;
                    fUnknown--;
                    const int mx = x + x0;
                    const int my = y + y0;
                    const bool w = map.walkable(mx, my);
                    if(w) continue;
                    bool inner = true;
                    for(int dx = -1; dx <= 1; dx++) {
                        for(int dy = -1; dy <= 1; dy++) {
                            if(dx == 0 && dy == 0) continue;
                            const int mx2 = mx + dx;
                            const int my2 = my + dy;
                            const bool w = map.walkable(mx2, my2);
                            if(w) {
                                inner = false;
                                break;
                            }
                        }
                        if(!inner) break;
                    }
                    if(inner) continue;
                    const float texX = xOffset + (x - y) * (fTileW / 2.0f);
                    const float texY = (x + y) * (fTileH / 2.0f);
                    p.drawTexture(texX, texY, fTileTex, eAlignment::hcenter);
                }
            }
        }

        const eResolution* fRes = nullptr;
        SDL_Renderer* fR = nullptr;

        bool fInitialized = false;

        int fTileW;
        int fTileH;

        int fUnknown = sAreaDim*sAreaDim;
        std::vector<std::vector<bool>> fKnown;
        std::shared_ptr<eTexture> fTileTex;
        std::shared_ptr<eTexture> fTex;
    };

    bool mShowMap = false;

    std::vector<std::vector<eMiniMapArea>> mAreas;

    int mAreaStrCounter = 0;
    std::shared_ptr<eTexture> mAreaStr;
};

#endif // EMINIMAP_H
