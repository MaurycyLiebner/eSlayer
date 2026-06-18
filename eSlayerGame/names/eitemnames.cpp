#include "eitemnames.h"

#include "../efileloader.h"

#include <eSlayerHelpers/eitem.h>
#include <eSlayerHelpers/eitemsdata.h>
#include <eSlayerHelpers/eitemaffixes.h>
#include <eSlayerHelpers/estringhelpers.h>

eItemNames eItemNames::sInstance;

std::string eItemNames::name(const eItemBase& item) {
    std::string result = name(item.fDataId);
    if(item.fPrefix) {
        const auto prefix = prefixName(item.fPrefix);
        result = eStringHelpers::replaceAll(prefix, "%1", result);
    }
    if(item.fSuffix) {
        const auto suffix = suffixName(item.fSuffix);
        result = eStringHelpers::replaceAll(suffix, "%1", result);
    }

    if(item.fSockets) {
        result += " [" + std::to_string(item.fSockets) + "]";
    }

    if(item.fType == eItemType::gold) {
        const auto countStr = std::to_string(item.fCount);
        result = eStringHelpers::replaceAll(result, "%1", countStr);
    }

    return result;
}

std::string eItemNames::name(const int itemDataId) {
    return sInstance.mNames[itemDataId];
}

std::string eItemNames::prefixName(const int id) {
    return sInstance.mPrefixNames[id];
}

std::string eItemNames::suffixName(const int id) {
    return sInstance.mSuffixNames[id];
}

bool eItemNames::load() {
    if(sInstance.mLoaded) return true;
    sInstance.mLoaded = true;
    return reload();
}

bool eItemNames::reload() {
    sInstance.mNames.clear();
    sInstance.mPrefixNames.clear();
    sInstance.mSuffixNames.clear();

    const auto dir = "Items";

    {
        const auto strMap = eFileLoader::loadNames(dir, "names");
        for(const auto& it : strMap) {
            const auto id = eItemsData::id(it.first);
            sInstance.mNames[id] = it.second;
        }
    }

    {
        const auto strMap = eFileLoader::loadNames(dir, "/Affixes/Prefixes/names");
        for(const auto& it : strMap) {
            const auto id = eItemAffixes::sPrefixes.id(it.first);
            sInstance.mPrefixNames[id] = it.second;
        }
    }

    {
        const auto strMap = eFileLoader::loadNames(dir, "/Affixes/Suffixes/names");
        for(const auto& it : strMap) {
            const auto id = eItemAffixes::sSuffixes.id(it.first);
            sInstance.mSuffixNames[id] = it.second;
        }
    }

    return true;
}
