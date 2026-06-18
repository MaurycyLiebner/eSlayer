#ifndef EOKCANCELBUTTON_H
#define EOKCANCELBUTTON_H

#include "../etexturecheckbutton.h"

class eOkButton : public eTextureCheckButton {
public:
    eOkButton(eMainWindow* const window);
};

class eCancelButton : public eTextureCheckButton {
public:
    eCancelButton(eMainWindow* const window);
};

#endif // EOKCANCELBUTTON_H
