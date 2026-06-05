#ifndef ERESOLUTION_H
#define ERESOLUTION_H

#include <string>
#include <vector>

struct eScreenSize {
    std::string fName;
    float fScaling;
    int fTileWidth;
    int fTileHeight;
    int fWidthBreakpoint;
    int fHeightBreakpoint;
};

struct eResolutionBase {
    int fWidth;
    int fHeight;

    std::string name() const {
        return std::to_string(fWidth) + "x" +
               std::to_string(fHeight);
    }

    static std::vector<eResolutionBase> sResolutions;

    bool operator==(const eResolutionBase other) const {
        return fWidth == other.fWidth &&
               fHeight == other.fHeight;
    }

    bool operator!=(const eResolutionBase other) const {
        return !(*this == other);
    }

    friend bool operator<(const eResolutionBase& r1,
                          const eResolutionBase& r2) {
        if(r1.fWidth != r2.fWidth) {
            return r1.fWidth < r2.fWidth;
        }
        return r1.fHeight < r2.fHeight;
    }
};

class eResolution : public eResolutionBase {
public:
    eResolution(const int width, const int height);

    int width() const { return fWidth; }
    int height() const { return fHeight; }

    float multiplier() const;

    int hugePadding() const;
    int largePadding() const;
    int smallPadding() const;
    int tinyPadding() const;
    int veryTinyPadding() const;
    int veryVeryTinyPadding() const;

    int margin() const;

    int extraHugeFontSize() const;
    int hugeFontSize() const;
    int largeFontSize() const;
    int smallFontSize() const;
    int verySmallFontSize() const;
    int tinyFontSize() const;

    int lineWidth() const;

    int tileWidth() const;
    int tileHeight() const;

    int centralWidgetLargeWidth() const;
    int centralWidgetSmallWidth() const;
    int centralWidgetLargeHeight() const;
    int centralWidgetSmallHeight() const;

    std::string textureSuffix() const;

    static void load();

    static std::vector<eScreenSize> sSizes;
private:
    eScreenSize mSize;
    using eResolutionBase::fWidth;
    using eResolutionBase::fHeight;
};

#endif // ERESOLUTION_H
