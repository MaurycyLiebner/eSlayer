#ifndef EGAMESCREEN_H
#define EGAMESCREEN_H

#include "../egameinput.h"
#include "../egameworld.h"
#include "../emaincharaction.h"
#include "../textures/echarunitmodel.h"
#include "../units/eunit.h"
#include "../widgets/gameScreen/egamepainter.h"
#include "escreenbase.h"

#include <eSlayerHelpers/epathfinder.h>
#include <eSlayerHelpers/epoint.h>
#include <eSlayerMapGenerator/emapgenerator.h>
#include <eSlayerServer/eserver.h>
#include <eSlayerHelpers/emissile.h>
#include <eSlayerHelpers/eunitarea.h>

class eUnitIndicator;
class ePlayerHealthIndicator;
class eSkillButton;
class eCheckButton;

class eGameScreen : public eScreenBase {
public:
    eGameScreen(eMainWindow* const window);
    ~eGameScreen();

    void setExitAction(const eAction& a);
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
    void openSkillMenu(const eAlignment align,
                       eSkillButton* const targetButton,
                       int& targetSkillVar);

    void updateMainCharFromServer(const eUnitData& u);

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

    eAction mExitAction;
    eWidget* mESCMenu = nullptr;
    eWidget* mDeadMenu = nullptr;
    eUnitIndicator* mUnitIndicator = nullptr;
    ePlayerHealthIndicator* mHealthIndicator = nullptr;
    ePlayerHealthIndicator* mManaIndicator = nullptr;
    ePlayerHealthIndicator* mStaminaIndicator = nullptr;
    ePlayerHealthIndicator* mExperienceIndicator = nullptr;

    eSkillButton* mLeftSkillButton = nullptr;
    eSkillButton* mRightSkillButton = nullptr;
    eWidget* mSkillMenu = nullptr;
    int mLeftSkill = 0;
    int mRightSkill = 0;
    eCheckButton* mRunButton = nullptr;
};

#endif // EGAMESCREEN_H
