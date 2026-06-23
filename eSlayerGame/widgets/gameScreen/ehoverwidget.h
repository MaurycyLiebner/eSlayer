#ifndef EHOVERWIDGET_H
#define EHOVERWIDGET_H

#include "../ewidget.h"

#include "../../textures/eiteminstancetexture.h"

#include <eSlayerHelpers/emodifier.h>
#include <eSlayerHelpers/eskills.h>

struct eEquipment;
struct eItem;
struct eAttributes;
struct eStats;

class eHoverWidget : public eWidget {
public:
    eHoverWidget(const eAttributes& attrs,
                 const eStats& stats,
                 eMainWindow* const w);
    ~eHoverWidget();

    using eDropAction = std::function<void()>;

    void initialize(const eDropAction& dropAction);

    void setGameTooltip(const std::string& text,
                        const SDL_Rect& rect = SDL_Rect{0, 0, 0, 0});
    void setGameTooltip(
        const std::vector<std::string>& text,
        const SDL_Rect& rect = SDL_Rect{0, 0, 0, 0});

    void setItem(const eItem& item);

    void setHoverItem(const eHoverItem& hitem,
                      const SDL_Rect& rect = SDL_Rect{0, 0, 0, 0});
    void setHoverSkill(const int skillId,
                       const bool showNextLevel,
                       const SDL_Rect& rect = SDL_Rect{0, 0, 0, 0});

    static eHoverWidget* sInstance;
    static void sUpdateDragItem(const eEquipment& eq);
    static void sSetHoverItem(
        const eHoverItem& hitem,
        const SDL_Rect& rect = SDL_Rect{0, 0, 0, 0});
    static void sSetHoverSkill(
        const int skillId, const bool showNextLevel,
        const SDL_Rect& rect = SDL_Rect{0, 0, 0, 0});
    static void sSetGameTooltip(
        const std::string& text,
        const SDL_Rect& rect = SDL_Rect{0, 0, 0, 0});
    static void sSetGameTooltip(
        const std::vector<std::string>& text,
        const SDL_Rect& rect = SDL_Rect{0, 0, 0, 0});
protected:
    void paintEvent(ePainter& p) override;
    bool mouseMoveEvent(const eMouseEvent& e) override;
    bool mousePressEvent(const eMouseEvent& e) override;
private:
    eModsCollection
    calculateTotalModifiers(
        const int skillId, const int levelId) const;

    const eAttributes& mAttrs;
    const eStats& mStats;

    eDropAction mDropAction;
    SDL_Point mMousePos;
    eItemInstanceTexture mItem;

    int mHoverSkillId = -1;
    int mHoverItemId = -1;
    SDL_Rect mHoverRect{0, 0, 0, 0};
    std::shared_ptr<eTexture> mHover;

    std::vector<std::string> mGameTooltip;
    SDL_Rect mGameHoverRect{0, 0, 0, 0};

    std::vector<std::string> mTooltip;
    std::shared_ptr<eTexture> mTooltipTex;
};

#endif // EHOVERWIDGET_H
