#ifndef ESKILLBUTTON_H
#define ESKILLBUTTON_H

#include "../ebuttonbase.h"

#include <eSlayerHelpers/eskillchoice.h>

class eSkillButton : public eButtonBase {
public:
    using eButtonBase::eButtonBase;
    ~eSkillButton();

    void initialize(const int schoice = -1);

    void setSkillId(const int skillId);
    int skillId() { return mSkillId; }

    void setLevelReq(const int levelReq);
    void setLevelReqMet(const bool met);

    void setTopRightText(const std::string& text);
    void setBottomRightText(const std::string& text);

    void setCooldown(const float time);
    void setCooldownMax(const float maxTime);

    void updateText();

    SDL_FPoint center() const;

    static std::map<int, int> sLeftMap;
    static std::map<int, int> sRightMap;
protected:
    bool mouseMoveEvent(const eMouseEvent& e) override;
    bool mouseLeaveEvent(const eMouseEvent& e) override;
    bool mouseEnterEvent(const eMouseEvent& e) override;
    bool keyPressEvent(const eKeyPressEvent& e) override;
    void paintEvent(ePainter& p) override;
private:
    void setText(eLabel*& ptr,
                 const std::string& text,
                 const eAlignment align);
    void setHotkey(const int fkey);
    void updateText(const std::map<int, int>& map);
    static void sRemoveHotkey(const int fkey);
    static void sRemoveHotkey(std::map<int, int>& map,
                              const int fkey);
    static void sAddHotkey(std::map<int, int>& map,
                           const int skillId,
                           const int fkey);

    static std::vector<eSkillButton*> sInstances;

    int mLevelReq = 1;
    bool mLevelReqMet = true;

    int mSchoice = -1;

    eLabel* mTopRight = nullptr;
    eLabel* mBottomRight = nullptr;

    int mSkillId = 0;

    float mCooldown = 0.f;
    float mMaxCooldown = 0.f;
};

#endif // ESKILLBUTTON_H
