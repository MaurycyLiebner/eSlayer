#ifndef ETEXTURECHECKBUTTON_H
#define ETEXTURECHECKBUTTON_H

#include "echeckablebutton.h"

class eTextureCheckButton : public eCheckableButton {
public:
    using eCheckableButton::eCheckableButton;

    void initialize(const std::shared_ptr<eTexture>& checked,
                    const std::shared_ptr<eTexture>& unchecked);
protected:
    void paintEvent(ePainter& p) override;
private:
    std::shared_ptr<eTexture> mChecked;
    std::shared_ptr<eTexture> mUnchecked;
};

#endif // ETEXTURECHECKBUTTON_H
