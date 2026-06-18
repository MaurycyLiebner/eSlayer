#include "eokcancelbutton.h"

#include "../../textures/euitextures.h"

eOkButton::eOkButton(eMainWindow* const window) :
    eTextureCheckButton(window) {
    initialize(eUITextures::sOkIcon,
               eUITextures::sOkIcon);
}

eCancelButton::eCancelButton(eMainWindow* const window) :
    eTextureCheckButton(window) {
    initialize(eUITextures::sCancelIcon,
               eUITextures::sCancelIcon);
}