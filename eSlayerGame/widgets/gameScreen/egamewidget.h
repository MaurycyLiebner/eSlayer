#ifndef EGAMEWIDGET_H
#define EGAMEWIDGET_H

#include "../elabel.h"

#include "../../egameinput.h"
#include "../../egameworld.h"
#include "../../emaincharaction.h"
#include "../../textures/echarunitmodel.h"
#include "../../units/eunit.h"
#include "egamepainter.h"

#include <eSlayerHelpers/epathfinder.h>
#include <eSlayerHelpers/epoint.h>
#include <eSlayerMapGenerator/emapgenerator.h>
#include <eSlayerServer/eserver.h>
#include <eSlayerHelpers/emissile.h>
#include <eSlayerHelpers/eunitarea.h>
#include <eSlayerHelpers/eunitdata.h>

class eUnitIndicator;

using eMainCharHandler = std::function<void(const eUnitData&)>;
using eDeathHandler = std::function<void()>;
using eRespawnHandler = std::function<void()>;

class eGameWidget : public eLabel {
public:
    eGameWidget(eMainWindow* const window);

    void initialize(const int clientId,
                    const std::shared_ptr<eServer>& server,
                    const std::shared_ptr<eMap>& map);

    int tileWidth() const { return mInput.tileWidth(); }
    int tileHeight() const { return mInput.tileHeight(); }

    int mapWidth() const { return mMap->width(); }
    int mapHeight() const { return mMap->height(); }

    const ePointF& characterPos() const;

    ePointF pixelToTilePos(const ePointF& pos,
                           const ePointF& pixel) const;
    ePointF pixelToTilePos(const ePointF& pixel) const;
    ePointF tilePosToPixel(const ePointF& pos) const;

    void setUnitIndicator(eUnitIndicator* const indicator);

    void setMenuVisible(const bool v) { mMenuVisible = v; }

    eMainCharAction& mainAction() { return mMainAction; }
    const std::shared_ptr<eUnit>& mainChar() const { return mMainChar; }
    const std::shared_ptr<eUnit>& highlightUnit() const { return mHighlightUnit; }
    const std::shared_ptr<eUnit>& pressedUnit() const { return mPressedUnit; }

    eGameInput& input() { return mInput; }
    const eGameInput& input() const { return mInput; }

    int leftSkill() const { return mLeftSkill; }
    void setLeftSkill(const int s) { mLeftSkill = s; }
    int rightSkill() const { return mRightSkill; }
    void setRightSkill(const int s) { mRightSkill = s; }

    const std::shared_ptr<eServer>& server() const { return mServer; }
    int clientId() const { return mClientId; }

    void setMainCharHandler(const eMainCharHandler& h) { mMainCharHandler = h; }
    void setDeathHandler(const eDeathHandler& h) { mDeathHandler = h; }
    void setRespawnHandler(const eRespawnHandler& h) { mRespawnHandler = h; }
protected:
    void paintEvent(ePainter& p) override;
    bool mousePressEvent(const eMouseEvent& e) override;
    bool mouseReleaseEvent(const eMouseEvent& e) override;
    bool mouseMoveEvent(const eMouseEvent& e) override;
private:
    void initializeTextures();
    void setHighlightedUnit(const std::shared_ptr<eUnit>& u);
    void setPressedUnit(const std::shared_ptr<eUnit>& u);

    eWalkable walkable() const;

    eGameWorld mWorld;
    eGameInput mInput;
    eGamePainter mGamePainter;

    int mClientId = -1;
    std::shared_ptr<eServer> mServer;

    eMainCharAction mMainAction;
    std::shared_ptr<eUnit> mMainChar;

    std::shared_ptr<eUnit> mHighlightUnit;
    std::shared_ptr<eUnit> mPressedUnit;
    std::shared_ptr<eMap> mMap;

    int mFrame = 0;

    bool mMenuVisible = false;

    eUnitIndicator* mUnitIndicator = nullptr;

    int mLeftSkill = 0;
    int mRightSkill = 0;

    eMainCharHandler mMainCharHandler;
    eDeathHandler mDeathHandler;
    eRespawnHandler mRespawnHandler;
};

#endif // EGAMEWIDGET_H
