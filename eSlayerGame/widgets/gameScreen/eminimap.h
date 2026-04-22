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

        void initialize(const int x0, const int y0,
                        const eMap& map) {
            if(fInitialized) return;
            fInitialized = true;

            const int id = eMapTextures::sTexs.id("reveal");
            fRevealTex = eMapTextures::sTexs.get(id);

            fTileW = fRes->tileWidth()/10;
            fTileH = fTileW/2;
            const int w = sAreaDim*fTileW;
            const int h = sAreaDim*fTileH;

            fTex = std::make_shared<eTexture>();
            fTex->create(fR, w, h);

            fClampTex = std::make_shared<eTexture>();
            fClampTex->create(fR, w, h, SDL_Color{255, 255, 255, 255});
            fClampTex->setBlendMode(SDL_BLENDMODE_ADD);

            fResultTex = std::make_shared<eTexture>();
            fResultTex->create(fR, w, h);
            fResultTex->setBlendMode(SDL_BLENDMODE_MUL);

            {
                const auto h = fTex->createTargetHolder(fR);
                ePainter p(fR);
                const int mw = map.width();
                const int mh = map.height();
                for(int dx = 0; dx < sAreaDim; dx++) {
                    const int x = x0 + dx;
                    if(x >= mw) break;
                    for(int dy = 0; dy < sAreaDim; dy++) {
                        const int y = y0 + dy;
                        if(y >= mh) break;
                        const bool w = map.walkable(x, y);
                        if(w) continue;
                        const int xx = fTex->width()/2 + (dx - dy) * (fTileW / 2.f);
                        const int yy = (dx + dy) * (fTileH / 2.f);
                        const SDL_Rect rect{xx, yy, fTileW, fTileH};
                        p.fillRect(rect, SDL_Color{0, 0, 0, 255});
                    }
                }
            }
        }

        void setKnown(ePointF pos,
                      const int x0, const int y0,
                      const eMap& map) {
            pos.fX -= x0;
            pos.fY -= y0;
            initialize(x0, y0, map);
            const auto h = fClampTex->createTargetHolder(fR);
            ePainter p(fR);

            const float xOffset = fClampTex->width()/2.f;
            const float texX = xOffset + (pos.fX - pos.fY) * (fTileW / 2.0f);
            const float texY = (pos.fX + pos.fY) * (fTileH / 2.0f);

            // const int dim = 8*maxDist;
            // p.fillRect(SDL_Rect{int(texX - dim/2.f),
            //                     int(texY - dim/2.f),
            //                     dim, dim},
            //            SDL_Color{0, 0, 0, 255});

            const float mult = fRes->multiplier();
            const float scale = mult;
            const float srcW = fRevealTex->width();
            const float srcH = fRevealTex->height();
            const float dstW = scale*srcW;
            const float dstH = scale*srcH;
            const SDL_FRect dstRect{texX - dstW/2.f,
                                    texY - dstH/2.f,
                                    dstW, dstH};
            const float texW = fClampTex->width();
            const float texH = fClampTex->height();
            const SDL_FRect texRect{0.f, 0.f, texW, texH};
            if(SDL_HasRectIntersectionFloat(&dstRect, &texRect)) {
                const SDL_FRect srcRect{0.f, 0.f, srcW, srcH};
                fRevealTex->render(fR, srcRect, dstRect);

                updateResultTex();
            }
        }

        void updateResultTex() {
            fResultTex->fill(fR, SDL_Color{255, 255, 255, 255});
            const auto h = fResultTex->createTargetHolder(fR);
            ePainter p(fR);
            p.drawTexture(0, 0, fTex);
            p.drawTexture(0, 0, fClampTex);
        }

        const eResolution* fRes = nullptr;
        SDL_Renderer* fR = nullptr;

        std::shared_ptr<eTexture> fRevealTex;

        bool fInitialized = false;

        int fTileW;
        int fTileH;

        std::shared_ptr<eTexture> fTex;
        std::shared_ptr<eTexture> fClampTex;
        std::shared_ptr<eTexture> fResultTex;
    };

    bool mShowMap = false;

    std::vector<std::vector<eMiniMapArea>> mAreas;

    int mAreaStrCounter = 0;
    std::shared_ptr<eTexture> mAreaStr;
};

#endif // EMINIMAP_H
