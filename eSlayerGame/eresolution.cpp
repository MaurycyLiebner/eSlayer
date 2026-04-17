#include "eresolution.h"

#include <cmath>

std::vector<eResolution> eResolution::sResolutions{
    eResolution{800, 600},
    eResolution{1024, 768},
    eResolution{1280, 720},
    eResolution{1280, 800},
    eResolution{1280, 1024},
    eResolution{1360, 768},
    eResolution{1366, 768},
    eResolution{1440, 900},
    eResolution{1600, 900},
    eResolution{1680, 1050},
    eResolution{1920, 1080},
    eResolution{1920, 1200},
    eResolution{2560, 1080},
    eResolution{2560, 1440},
    eResolution{2560, 1600},
    eResolution{3440, 1440},
    eResolution{3840, 2160},
};

eResolution::eResolution(const int width, const int height) :
    mWidth(width), mHeight(height) {
    if(height <= 800 || width <= 1280) {
        mUIScale = eUIScale::small;
    } else if(height <= 1200) {
        mUIScale = eUIScale::medium;
    } else if(height <= 1600) {
        mUIScale = eUIScale::large;
    } else {
        mUIScale = eUIScale::huge;
    }
    mName = std::to_string(mWidth) + "x" + std::to_string(mHeight);
}

int eResolution::hugePadding() const {
    return std::round(15.f*multiplier());
}

int eResolution::largePadding() const {
    return std::round(10.f*multiplier());
}

int eResolution::smallPadding() const {
    return std::round(7.5f*multiplier());
}

int eResolution::tinyPadding() const {
    return std::round(5.f*multiplier());
}

int eResolution::veryTinyPadding() const {
    return std::round(2.5f*multiplier());
}

int eResolution::veryVeryTinyPadding() const {
    return std::round(1.67f*multiplier());
}

int eResolution::margin() const {
    return largePadding();
}

float eResolution::multiplier() const {
    switch(mUIScale) {
    case eUIScale::small:
        return 1.0f;
    case eUIScale::medium:
        return 1.5f;
    case eUIScale::large:
        return 2.0f;
    case eUIScale::huge:
        return 3.0f;
    }
    return 1.f;
}

int eResolution::extraHugeFontSize() const {
    return std::round(60*multiplier());
}

int eResolution::hugeFontSize() const {
    return std::round(30*multiplier());
}

int eResolution::largeFontSize() const {
    return std::round(20*multiplier());
}

int eResolution::smallFontSize() const {
    return std::round(15*multiplier());
}

int eResolution::verySmallFontSize() const {
    return std::round(12*multiplier());
}

int eResolution::tinyFontSize() const {
    return std::round(11.33f*multiplier());
}

int eResolution::lineWidth() const {
    return std::max(1, largeFontSize()/15);
}

int eResolution::tileWidth() const {
    switch(mUIScale) {
    case eUIScale::small:
        return 160;
    case eUIScale::medium:
        return 240;
    case eUIScale::large:
        return 320;
    case eUIScale::huge:
        return 480;
    }
    return 160;
}

int eResolution::tileHeight() const {
    switch(mUIScale) {
    case eUIScale::small:
        return 79;
    case eUIScale::medium:
        return 119;
    case eUIScale::large:
        return 159;
    case eUIScale::huge:
        return 239;
    }
    return 79;
}

int eResolution::centralWidgetLargeWidth() const {
    return 660*multiplier();
}

int eResolution::centralWidgetSmallWidth() const {
    return 480*multiplier();
}

int eResolution::centralWidgetLargeHeight() const {
    return 540*multiplier();
}

int eResolution::centralWidgetSmallHeight() const {
    return 400*multiplier();
}

std::string eResolution::textureSuffix() const {
    switch(mUIScale) {
    case eUIScale::small:
        return "_small";
    case eUIScale::medium:
        return "_medium";
    case eUIScale::large:
        return "_large";
    case eUIScale::huge:
        return "_huge";
    }
    return "_small";
}
