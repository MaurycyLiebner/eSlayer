#ifndef EBOTTOMWIDGET_H
#define EBOTTOMWIDGET_H

#include "../ewidget.h"

class eSkillButton;
class ePlayerHealthIndicator;
class eTextureCheckButton;
class eInventoryBagpackWidget;
struct eStats;
struct eEquipment;

class eBottomWidget : public eWidget {
public:
    eBottomWidget(const eStats& stats,
                  eEquipment& eq,
                  eMainWindow* const window);

    using eCheckAction = std::function<void(const bool check)>;
    void initialize(const eAction& leftSkillA,
                    const int leftSkillId,
                    const eAction& rightSkillA,
                    const int rightSkillId,
                    const eCheckAction& runA,
                    const eCheckAction& portalA,
                    const eCheckAction& invA,
                    const eCheckAction& attrsA,
                    const eCheckAction& skillA,
                    const eCheckAction& partyA);

    void setLeftSkill(const int skillId);
    void setRightSkill(const int skillId);

    void setIndicators(const int health,
                       const int maxHealth,
                       const int mana,
                       const int maxMana,
                       const int stamina,
                       const int maxStamina,
                       const bool staminaPotion,
                       const int exp,
                       const int maxExp);

    void setRunning(const bool r);

    eSkillButton* leftSkillButton() const
    { return mLeftSkillButton; }
    eSkillButton* rightSkillButton() const
    { return mRightSkillButton; }

    bool dropItem() const;

    void switchBeltVisible();
protected:
    void paintEvent(ePainter& p) override;
private:
    void hideBeltExt();
    void showBeltExt();

    const eStats& mStats;
    eEquipment& mEq;

    eSkillButton* mLeftSkillButton = nullptr;
    eSkillButton* mRightSkillButton = nullptr;

    ePlayerHealthIndicator* mExperienceIndicator = nullptr;
    ePlayerHealthIndicator* mHealthIndicator = nullptr;
    ePlayerHealthIndicator* mManaIndicator = nullptr;
    ePlayerHealthIndicator* mStaminaIndicator = nullptr;

    eTextureCheckButton* mRunButton = nullptr;

    eInventoryBagpackWidget* mBelt = nullptr;
    bool mBeltExtTmp = false;
    eInventoryBagpackWidget* mBeltExt = nullptr;
};

#endif // EBOTTOMWIDGET_H
