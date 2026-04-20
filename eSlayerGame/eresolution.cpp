#include "eresolution.h"

#include <eSlayerHelpers/eexceptions.h>
#include <eSlayerHelpers/efileloaderbase.h>

#include <cmath>

std::vector<eScreenSize> eResolution::sSizes;

std::vector<eResolutionBase>
eResolutionBase::sResolutions{
    eResolutionBase{800, 600},
    eResolutionBase{1024, 768},
    eResolutionBase{1280, 720},
    eResolutionBase{1280, 800},
    eResolutionBase{1280, 1024},
    eResolutionBase{1360, 768},
    eResolutionBase{1366, 768},
    eResolutionBase{1440, 900},
    eResolutionBase{1600, 900},
    eResolutionBase{1680, 1050},
    eResolutionBase{1920, 1080},
    eResolutionBase{1920, 1200},
    eResolutionBase{2560, 1080},
    eResolutionBase{2560, 1440},
    eResolutionBase{2560, 1600},
    eResolutionBase{3440, 1440},
    eResolutionBase{3840, 2160},
};

eResolution::eResolution(const int width, const int height) {
    fWidth = width;
    fHeight = height;

    mSize = sSizes[0];
    for(int i = 0; i < sSizes.size(); i++) {
        const auto& size = sSizes[i];
        const int wb = size.fWidthBreakpoint;
        const int hb = size.fHeightBreakpoint;
        if((width <= wb || height <= hb) ||
           (wb == 0 && hb == 0)) {
            mSize = sSizes[i];
            break;
        }
    }
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
    return mSize.fScaling;
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
    return mSize.fTileWidth;
}

int eResolution::tileHeight() const {
    return mSize.fTileHeight;
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
    return "_" + mSize.fName;
}

void eResolution::load() {
    const auto dir = "Textures";
    const auto jdata = eFileLoaderBase::parse(dir, "sizes.json");
    for(auto& [key, value] : jdata.items()) {
        auto& size = sSizes.emplace_back();
        size.fName = key;
        size.fScaling = value.value("scaling", 1.f);
        size.fTileWidth = value["tileWidth"];
        size.fTileHeight = value["tileHeight"];
        size.fWidthBreakpoint = value.value("widthBreakpoint", 0);
        size.fHeightBreakpoint = value.value("heightBreakpoint", 0);
    }
    if(sSizes.empty()) eRuntimeThrow("No sizes in \"Textures/sizes.json\"");
}
