#ifndef EITEMDRAGWIDGET_H
#define EITEMDRAGWIDGET_H

#include "../ewidget.h"

#include <eSlayerHelpers/emodifier.h>

struct eEquipment;
struct eItem;
struct eAttributes;
struct eStats;

class eItemDragWidget : public eWidget {
public:
    eItemDragWidget(const eAttributes& attrs,
                    const eStats& stats,
                    eMainWindow* const w);
    ~eItemDragWidget();

    using eDropAction = std::function<void()>;

    void initialize(const eDropAction& dropAction);

    void setItemDataId(const int dataId);

    void setHoverItem(const eItem& item);
    void setHoverSkill(const int skillId,
                       const bool showNextLevel);

    static eItemDragWidget* sInstance;
    static void sUpdateDragItem(const eEquipment& eq);
    static void sSetHoverItem(const eItem& item);
    static void sSetHoverSkill(const int skillId,
                               const bool showNextLevel);
protected:
    void paintEvent(ePainter& p) override;
    bool mouseMoveEvent(const eMouseEvent& e) override;
    bool mousePressEvent(const eMouseEvent& e) override;
private:
    std::map<eModifierType, eModifier>
    calculateTotalModifiers(const int skillId, const int levelId,
                            int& count, float& cooldown, float& manaCost) const;

    const eAttributes& mAttrs;
    const eStats& mStats;

    eDropAction mDropAction;
    SDL_Point mMousePos;
    std::shared_ptr<eTexture> mItem;

    int mHoverSkillId = -1;
    int mHoverItemId = -1;
    std::shared_ptr<eTexture> mHover;
};

#endif // EITEMDRAGWIDGET_H
