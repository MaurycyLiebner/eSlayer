#include "equesttext.h"

#include "../efileloader.h"

eQuestText eQuestText::sInstance;

const std::string& eQuestText::title(
    const std::string& name) {
    return sInstance.mValues[name];
}

const std::string& eQuestText::text(
    const std::string& name,
    const uint8_t stage) {
    const auto stageStr = std::to_string(stage);
    return sInstance.mValues[name + "Text" + stageStr];
}

bool eQuestText::load() {
    if(sInstance.mLoaded) return true;
    sInstance.mLoaded = true;
    return reload();
}

bool eQuestText::reload() {
    const auto dir = "Quests";
    sInstance.mValues = eFileLoader::loadNames(
        dir, "text");
    return true;
}