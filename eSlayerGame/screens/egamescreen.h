#ifndef EGAMESCREEN_H
#define EGAMESCREEN_H

#include "escreenbase.h"

#include "../textures/echarunitmodel.h"
#include "../textures/elightingtexture.h"

#include <eSlayerMapGenerator/emapgenerator.h>
#include <eSlayerHelpers/epathfinder.h>
#include <eSlayerHelpers/epoint.h>
#include <eSlayerHelpers/emovementhandler.h>
#include <eSlayerServer/eserver.h>

#include "../units/eunit.h"

class eGameScreen : public eScreenBase {
public:
    using eScreenBase::eScreenBase;
    ~eGameScreen();

    void setExitAction(const eAction& a);
    void initialize(const int clientId,
                    const std::shared_ptr<eServer>& server,
                    const std::shared_ptr<eMap>& map);

    int tileWidth() const { return mTileW; }
    int tileHeight() const { return mTileH; }

    int mapWidth() const { return mMap->width(); }
    int mapHeight() const { return mMap->height(); }

    const ePointF& characterPos() const;

    ePointF pixelToTilePos(const ePointF& pos,
                           const ePointF& pixel) const;
    ePointF pixelToTilePos(const ePointF& pixel) const;
    ePointF tilePosToPixel(const ePointF& pos) const;
protected:
    void paintEvent(ePainter& p) override;
    bool mousePressEvent(const eMouseEvent& e) override;
    bool mouseReleaseEvent(const eMouseEvent& e) override;
    bool mouseMoveEvent(const eMouseEvent& e) override;
    bool keyPressEvent(const eKeyPressEvent& e) override;
private:
    void initializeTextures();
    void showESCMenu();
    void hideESCMenu();

    void updateTargetPos();
    void setTargetPos(const ePointF& pos);

    std::shared_ptr<eTexture> mBaseTex;
    std::shared_ptr<eLightingTexture> mLightingTex;
    std::shared_ptr<eTexture> mDisplayTex;

    int mClientId = -1;
    std::shared_ptr<eServer> mServer;

    eCharUnitModel mModel;
    std::vector<std::shared_ptr<eUnit>> mUnits;
    std::map<int, int> mUnitIndexMap;
    std::shared_ptr<eMap> mMap;
    int mFrame = 0;

    const int mTileW = 160;
    const int mTileH = 79;

    bool mMousePressed = false;
    ePointF mMousePos{0., 0.};

    eMovementHandler mMovementHandler;

    eAction mExitAction;
    eWidget* mESCMenu = nullptr;
};

#endif // EGAMESCREEN_H
