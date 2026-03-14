#ifndef EGAMESCREEN_H
#define EGAMESCREEN_H

#include "escreenbase.h"

#include "../textures/echarunitmodel.h"
#include "../widgets/gameScreen/egamepainter.h"

#include <eSlayerMapGenerator/emapgenerator.h>
#include <eSlayerHelpers/epathfinder.h>
#include <eSlayerHelpers/epoint.h>
#include <eSlayerServer/eserver.h>

#include "../units/eunit.h"
#include "../emaincharaction.h"

#include <set>

class eUnitIndicator;
class ePlayerHealthIndicator;
class eButtonBase;
class eCheckButton;

class eGameScreen : public eScreenBase {
public:
    eGameScreen(eMainWindow* const window);
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
    void showDeadMenu();
    void showESCMenu();
    void hideESCMenu();
    void setHighlightedUnit(const std::shared_ptr<eUnit>& u);
    void setPressedUnit(const std::shared_ptr<eUnit>& u);

    eWalkable walkable() const;

    eGamePainter mGamePainter;

    int mClientId = -1;
    std::shared_ptr<eServer> mServer;

    eMainCharAction mMainAction;
    std::shared_ptr<eUnit> mMainChar;

    std::vector<std::shared_ptr<eUnit>> mUnits;
    std::shared_ptr<eUnit> mHighlightUnit;
    std::shared_ptr<eUnit> mPressedUnit;
    std::map<int, int> mUnitIndexMap;
    std::set<int> mUnitSlots;
    std::shared_ptr<eMap> mMap;

    int mFrame = 0;

    const int mTileW = 160;
    const int mTileH = 79;

    bool mMousePressed = false;
    ePointF mMousePos{0., 0.};

    eAction mExitAction;
    eWidget* mESCMenu = nullptr;
    eWidget* mDeadMenu = nullptr;
    eUnitIndicator* mUnitIndicator = nullptr;
    ePlayerHealthIndicator* mHealthIndicator = nullptr;
    ePlayerHealthIndicator* mManaIndicator = nullptr;
    ePlayerHealthIndicator* mStaminaIndicator = nullptr;
    ePlayerHealthIndicator* mExperienceIndicator = nullptr;

    eButtonBase* mLeftSkillButton = nullptr;
    eButtonBase* mRightSkillButton = nullptr;
    eCheckButton* mRunButton = nullptr;
};

#endif // EGAMESCREEN_H
