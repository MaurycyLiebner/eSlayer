#ifndef EMINIMAPAREA_H
#define EMINIMAPAREA_H

#include "../../textures/emaptextures.h"

#include <eSlayerHelpers/epoint.h>

class eMap;
class ePainter;

struct eMiniMapArea {
    static const int sAreaDim = 10;
    eMiniMapArea(const eResolution* res, SDL_Renderer* const r);

    void initialize();

    void setKnown(ePointF pos, const int x0, const int y0, eMap& map);

    bool setKnown(ePainter& p, const int x0, const int y0, const int x, const int y, eMap& map,
                  const float xOffset);

    bool tryFillingPastKnown(const int x0, const int y0, eMap& map);

    const eResolution* fRes = nullptr;
    SDL_Renderer* fR = nullptr;

    bool fInitialized = false;

    int fTileW;
    int fTileH;

    int fUnknown = sAreaDim*sAreaDim;
    std::vector<std::vector<bool>> fKnown;
    std::shared_ptr<eTexture> fTex;

    std::vector<std::vector<bool>> fPastKnown;
};

#endif // EMINIMAPAREA_H
