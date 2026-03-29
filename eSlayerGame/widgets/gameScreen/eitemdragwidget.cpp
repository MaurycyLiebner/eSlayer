#include "eitemdragwidget.h"

#include "../../textures/eitemstextures.h"
#include "../../textures/etextgenerator.h"

#include <eSlayerHelpers/eitemsdata.h>
#include <eSlayerHelpers/eequipment.h>

eItemDragWidget* eItemDragWidget::sInstance = nullptr;

eItemDragWidget::eItemDragWidget(eMainWindow* const w) :
    eWidget(w) {
    sInstance = this;
}

eItemDragWidget::~eItemDragWidget() {
    sInstance = nullptr;
}

void eItemDragWidget::initialize(const eDropAction& dropAction) {
    mDropAction = dropAction;
}

void eItemDragWidget::setItemDataId(const int dataId) {
    if(dataId == -1) {
        mItem = nullptr;
    } else {
        const auto r = renderer();
        const auto name = eItemsData::name(dataId);
        auto& itemTex = eItemsTextures::get(name);
        itemTex.request(r);
        mItem = itemTex.fTex;
    }
}

void eItemDragWidget::setHoverItem(const eItem& item) {
    if(item.fType == eItemType::none) {
        mHover = nullptr;
    } else if(!mHover || item.fItemId != mHoverItemId) {
        mHoverItemId = item.fItemId;
        int totalHeight = 0;
        int maxWidth = 0;
        std::vector<std::shared_ptr<eTexture>> lines;
        const auto r = renderer();
        const auto res = resolution();
        const int fontSize = res.smallFontSize();
        const auto font = eFonts::textFont(fontSize);
        const auto addText = [&](const std::string& text) {
            eTextGenerator gen(r, eFontColor::white, font);
            const auto tex = gen.generate(text);
            totalHeight += tex->height();
            maxWidth = std::max(maxWidth, tex->width());
            lines.emplace_back(tex);
        };
        addText(eItemsData::name(item.fDataId));

        mHover = std::make_shared<eTexture>();
        mHover->create(r, maxWidth, totalHeight);
        {
            const auto h = mHover->createTargetHolder(r);
            ePainter p(r);
            int y = 0;
            for(const auto& l : lines) {
                p.drawTexture(maxWidth/2, y, l, eAlignment::hcenter);
                y += l->height();
            }
        }
    }
}

void eItemDragWidget::sUpdateDragItem(const eEquipment& eq) {
    if(!sInstance) return;
    if(eq.fDragged.fType == eItemType::none) {
        sInstance->setItemDataId(-1);
    } else {
        const int dataId = eq.fDragged.fDataId;
        sInstance->setItemDataId(dataId);
    }
}

void eItemDragWidget::sSetHoverItem(const eItem& item) {
    sInstance->setHoverItem(item);
}

void eItemDragWidget::paintEvent(ePainter& p) {
    if(mItem) {
        p.drawTexture(mMousePos.x, mMousePos.y,
                      mItem, eAlignment::center);
    } else if(mHover) {
        const auto res = resolution();
        const int margin = 10*res.multiplier();
        const SDL_Rect rect{mMousePos.x - mHover->width()/2,
                            mMousePos.y - mHover->height()/2,
                            mHover->width(), mHover->height()};
        const SDL_Rect fillRect{rect.x - margin, rect.y - margin,
                                rect.w + 2*margin, rect.h + 2*margin};
        p.fillRect(fillRect, SDL_Color{0, 0, 0, 128});
        p.drawTexture(rect, mHover, eAlignment::center);
    }
}

bool eItemDragWidget::mouseMoveEvent(const eMouseEvent& e) {
    mMousePos.x = e.x();
    mMousePos.y = e.y();
    return mItem.get();
}

bool eItemDragWidget::mousePressEvent(const eMouseEvent& e) {
    if(!mItem) return false;
    mDropAction(mMousePos);
    return true;
}
