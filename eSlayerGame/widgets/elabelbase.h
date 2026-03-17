#ifndef ELABELBASE_H
#define ELABELBASE_H

#include "ewidget.h"
#include "efonts.h"

class eLabelBase {
public:
    eLabelBase(eMainWindow* const window);

    bool setTinyFontSize();
    bool setVerySmallFontSize();
    bool setSmallFontSize();
    bool setLargeFontSize();
    bool setHugeFontSize();
    bool setExtraHugeFontSize();

    bool setFont(const eFont& font);
    bool setFontSize(const int s);
    std::string text() const { return mText; }
    bool setText(const std::string& text);
    bool setTexture(const std::shared_ptr<eTexture>& tex);
    bool setFontColor(const eFontColor color);

    eFontColor fontColor() const
    { return mFontColor; }

    int fontSize() const;

    void setWrapWidth(const int w);

    eAlignment textAlignment() { return mTextAlign; }
    void setTextAlignment(const eAlignment a) { mTextAlign = a; }

    int lineWidth() const;

    void textureSize(int& w, int& h) const;
protected:
    const std::shared_ptr<eTexture>& texture();
    bool updateTexture();
private:
    const eResolution& res() const;
    bool updateTextTexture();

    int mWidth = 0;
    eMainWindow* const mWindow;
    eFont mFont;
    std::string mText;
    std::shared_ptr<eTexture> mTexture;
    bool mUpdateTextTextureFailed = false;
    eFontColor mFontColor = eFontColor::white;
    eAlignment mTextAlign = eAlignment::center;
};

#endif // ELABELBASE_H
