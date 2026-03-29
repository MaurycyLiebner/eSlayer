#ifndef EITEMDRAGWIDGET_H
#define EITEMDRAGWIDGET_H

#include "../ewidget.h"

struct eEquipment;

class eItemDragWidget : public eWidget {
public:
    eItemDragWidget(eMainWindow* const w);
    ~eItemDragWidget();

    using eDropAction = std::function<void(const SDL_Point& pos)>;

    void initialize(const eDropAction& dropAction);

    void setItemDataId(const int dataId);

    static eItemDragWidget* sInstance;
    static void sUpdateDragItem(const eEquipment& eq);
protected:
    void paintEvent(ePainter& p) override;

    bool mouseMoveEvent(const eMouseEvent& e) override;

    bool mousePressEvent(const eMouseEvent& e) override;
private:
    eDropAction mDropAction;
    SDL_Point mMousePos;
    std::shared_ptr<eTexture> mItem;
};

#endif // EITEMDRAGWIDGET_H
