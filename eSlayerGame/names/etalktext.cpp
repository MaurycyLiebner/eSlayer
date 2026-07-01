#include "etalktext.h"

#include "../efileloader.h"

eTalkText eTalkText::sInstance;

const std::string& eTalkText::title(
    const std::string& name) {
    return sInstance.mValues[name];
}

const std::string& eTalkText::text(
    const std::string& name) {
    return sInstance.mValues[name + "Text"];
}

bool eTalkText::load() {
    if(sInstance.mLoaded) return true;
    sInstance.mLoaded = true;
    return reload();
}

bool eTalkText::reload() {
    const auto dir = "Quests";
    sInstance.mValues = eFileLoader::loadNames(
        dir, "Talk/text");
    return true;
}
