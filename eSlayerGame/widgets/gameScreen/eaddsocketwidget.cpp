#include "eaddsocketwidget.h"

#include "eitemplacewidget.h"
#include "eokcancelbutton.h"
#include "einventorywidget.h"

#include "../../textures/euitextures.h"

#include <eSlayerHelpers/eequipment.h>

void eAddSocketWidget::initialize(
    const uint32_t clientId,
    eEquipment& eq,
    const eTryAction& applyAction,
    const eAction& cancelAction) {
    const auto& res = resolution();
    const int p = res.largePadding();

    const auto innerW = new eWidget(window());
    innerW->setNoPadding();

    mPlace = new eItemPlaceWidget(window());
    mPlace->intialize(clientId,
                      eUITextures::sWeaponSlot,
                      eq, nullptr,
                      &eEquipment::fTemporary,
                      ePlaceType::temporary,
                      eHoverItemType::regular);
    innerW->addWidget(mPlace);

    const auto buttonsW = new eWidget(window());
    buttonsW->setNoPadding();
    const auto cancelButton = new eCancelButton(window());
    buttonsW->addWidget(cancelButton);
    cancelButton->setCheckAction([cancelAction](const bool) {
        if(cancelAction) cancelAction();
    });
    const auto okButton = new eOkButton(window());
    okButton->setCheckAction([applyAction, okButton](const bool) {
        if(applyAction) {
            const bool r = applyAction();
            if(r) okButton->hide();
        }
    });
    buttonsW->addWidget(okButton);
    buttonsW->fitHeight();
    buttonsW->setWidth(mPlace->width());
    buttonsW->layoutHorizontallyWithoutSpaces();
    innerW->addWidget(buttonsW);

    innerW->stackVertically(p);
    innerW->fitContent();

    setup(innerW);
}

bool eAddSocketWidget::dropItem() {
    return mPlace->dropItem();
}
