#include "elineedit.h"

#include <algorithm>

eLineEdit::eLineEdit(eMainWindow* const window) :
    eLabel(window) {
    setTextAlignment(eAlignment::left | eAlignment::vcenter);
}

void eLineEdit::setChangeAction(const eAction a) {
    mChangeAction = a;
}

void eLineEdit::setMaxLength(const int max) {
    mMaxLength = max;
}

void eLineEdit::setMaxLengthAndFit(const int max) {
    setMaxLength(max);
    const auto tmp = text();
    setText(std::string(max, 'A'));
    fitContent();
    setText(tmp);
}

void eLineEdit::allow(const char c) {
    mAllowed.push_back(c);
}

void eLineEdit::disallow(const char c) {
    std::string allowed;
    for(const auto cc : mAllowed) {
        if(cc == c) continue;
        allowed.push_back(cc);
    }
    std::swap(mAllowed, allowed);
}

void eLineEdit::setAllowed(const std::string& a) {
    mAllowed = a;
}

bool eLineEdit::mouseMoveEvent(const eMouseEvent& e) {
    (void)e;
    return true;
}

bool eLineEdit::mouseEnterEvent(const eMouseEvent& e) {
    (void)e;
    mHovered = true;
    return true;
}

bool eLineEdit::mouseLeaveEvent(const eMouseEvent& e) {
    (void)e;
    mHovered = false;
    return true;
}

bool eLineEdit::keyPressEvent(const eKeyPressEvent& e) {
    const auto k = e.key();
    if(k == SDL_Scancode::SDL_SCANCODE_BACKSPACE) {
        auto txt = text();
        if(txt.empty()) return true;
        txt.pop_back();
        setText(txt);
        if(mChangeAction) mChangeAction();
    } else {
        return false;
    }
    return true;
}

bool eLineEdit::textInputEvent(const eTextInputEvent& e) {
    const auto& txt = eLineEdit::text();
    if(txt.length() >= mMaxLength) return true;
    const auto text = e.text();
    if(mAllowed.find(text) == std::string::npos) return true;
    setText(txt + text);
    if(mChangeAction) mChangeAction();
    return true;
}

void eLineEdit::paintEvent(ePainter& p) {
    const auto rect = eWidget::rect();
    p.fillRect(rect, SDL_Color{0, 0, 0, 255});
    p.drawRect(rect, SDL_Color{255, 255, 255, 255}, lineWidth());
    const auto& tex = texture();
    if(tex) {
        auto r = rect;
        {
            const auto& res = resolution();
            const int p = res.smallPadding();
            r.x += p;
            r.y += p;
            r.w -= 2*p;
            r.h -= 2*p;
        }
        p.drawTexture(r, tex, textAlignment());
    }
    if(!mHovered && !isKeyboardGrabber() && !isLastPressed()) return;
    mTime++;
    if(mTime > 10) {
        if(mTime > 20) {
            mTime = 0;
        }
        return;
    }

    int texw;
    int texh;
    const int fs = fontSize();
    if(tex) {
        texw = tex->width();
        texh = tex->height();
    } else {
        texw = 0;
        texh = fs;
    }
    const int lh = 2*texh/3;
    const int x = texw + fs/4;
    const int y = (height() + lh)/2;
    const int lw = lineWidth();
    const int w = 4*lw;
    const int h = 2*lw;
    SDL_Color col1;
    SDL_Color col2;
    const auto fcolor = fontColor();
    eFontColorHelpers::colors(fcolor, col1, col2);
    p.fillRect({x, y, w, h}, col1);
}

void eLineEdit::sizeHint(int& w, int& h) {
    eLabel::sizeHint(w, h);
    const auto& res = resolution();
    const int p = res.smallPadding();
    w += 2*p;
    h += 2*p;
}
