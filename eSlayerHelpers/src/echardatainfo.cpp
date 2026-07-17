#include "eSlayerHelpers/echardatainfo.h"

#include "eSlayerHelpers/efileloaderbase.h"
#include "eSlayerHelpers/eexceptions.h"

eCharDataInfo eCharDataInfo::sInstance;

eCharData& eCharDataInfo::get(const std::string& name) {
    return sInstance.getImpl(name);
}

eCharData& eCharDataInfo::get(const int id) {
    return sInstance.getImpl(id);
}

int eCharDataInfo::id(const std::string& name) {
    return sInstance.idImpl(name);
}

void eCharDataInfo::load() {
    return sInstance.loadImpl();
}

void eCharDataInfo::loadImpl() {
    if(mLoaded) return;
    mLoaded = true;

    const auto dir = "Textures";

    std::vector<std::string> chars;
    try {
        const auto jdata = eFileLoaderBase::parse(dir, "units/units.json");
        chars = jdata.get<std::vector<std::string>>();
    } catch(...) {
        eRuntimeThrow("Failed to parse " + dir + "/units/units.json");
    }

    mChars.reserve(chars.size());
    for(const auto& name : chars) {
        try {
            eCharData texs;
            const int id = mChars.nextId();
            texs.setTypeId(id);
            texs.setName(name);
            auto jdata = eFileLoaderBase::parse(dir, "units/" + name + "/" + name + ".json");
            texs.load(jdata);
            mChars.add(name, texs);
        } catch(const std::exception& e) {
            eExceptions::showDialog(e);
        }
    }
}

eCharData& eCharDataInfo::getImpl(const std::string &name) {
    const int id = mChars.id(name);
    return getImpl(id);
}

eCharData& eCharDataInfo::getImpl(const int id) {
    if(id < 0 || id >= mChars.size()) {
        eRuntimeThrow("Index out of range.");
    }
    return mChars.get(id);
}

int eCharDataInfo::idImpl(const std::string& name) const {
    return mChars.id(name);
}
