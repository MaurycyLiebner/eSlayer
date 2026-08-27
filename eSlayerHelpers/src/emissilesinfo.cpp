#include "eSlayerHelpers/emissilesinfo.h"

#include "eSlayerHelpers/efileloaderbase.h"

#include "eSlayerHelpers/eskills.h"
#include "eSlayerHelpers/eitemsdata.h"
#include "eSlayerHelpers/esoundsbase.h"

eStringIdMapVector<eMissileInfo>
eMissilesInfo::sMissiles;
int eMissilesInfo::sFleshId = -1;
int eMissilesInfo::sIceId = -1;

bool eMissilesInfo::sLoaded = false;

int eMissileInfo::nFrames(const int animId) const {
    const auto& anim = mAnims.get(animId);
    return anim.fNFrames;
}

int eMissileInfo::nDirs(const int animId) const {
    const auto& anim = mAnims.get(animId);
    return anim.fNDirs;
}

int eMissileInfo::animId(const std::string& name) const {
    return mAnims.id(name);
}

const eMissileAnim& eMissileInfo::anim(const int id) const {
    return mAnims.get(id);
}

void eMissilesInfo::load() {
    if(sLoaded) return;
    sLoaded = true;

    const std::string dir = "Textures";

    const auto jdata = eFileLoaderBase::parse(dir, "missiles/missiles.json");
    const auto missiles = jdata.get<std::vector<std::string>>();

    sMissiles.reserve(missiles.size() + 1);
    sMissiles.add("none", eMissileInfo());
    for(const auto& name : missiles) {
        const auto pathBase = "missiles/" + name + "/" + name;
        const auto jdata = eFileLoaderBase::parse(dir, pathBase + ".json");
        const int dirs = jdata["directions"];
        const float lighting = jdata.value("lighting", 0.f);
        const auto typeStr = jdata.value("type", "regular");
        const float radius = jdata.value("radius", 0.f);
        eMissileType type;
        if(typeStr == "explosion") {
            type = eMissileType::explosion;
        } else if(typeStr == "overlay") {
            type = eMissileType::overlay;
        } else if(typeStr == "regular") {
            type = eMissileType::regular;
        } else if(typeStr == "curse") {
            type = eMissileType::curse;
        } else if(typeStr == "aura") {
            type = eMissileType::aura;
        } else {
            eRuntimeThrow("Unknown missile type \"" + typeStr + "\" in " +
                          dir + "/" + pathBase + ".json");
        }
        const auto& anims = jdata["animations"];
        eMissileInfo missile;
        missile.mLighting = lighting;
        missile.mType = type;
        missile.mRadius = radius;
        for(auto& [aname, animData] : anims.items()) {
            eMissileAnim anim;
            anim.fNFrames = animData.value("frames", 0);
            anim.fNDirs = dirs;
            anim.fPath = pathBase + "_" + aname;
            if(animData.contains("overwrite")) {
                const auto overwriteStr = animData.value("overwrite", "");
                const int id = missile.mAnims.id(overwriteStr);
                if(id < 0) {
                    eRuntimeThrow("Unrecognized overwrite \"" + overwriteStr + "\".");
                }
                anim.fOverwriteId = id;
            }
            missile.mAnims.add(aname, anim);
        }
        missile.mAppearAnimId = missile.animId("appear");
        missile.mBaseAnimId = missile.animId("base");
        missile.mDisappearAnimId = missile.animId("disappear");
        missile.mHitAnimId = missile.animId("hit");
        missile.mStayAnimId = missile.animId("stay");

        const auto parseSound = [&](const std::string& name) {
            const auto soundStr = jdata.value(name, "");
            if(soundStr.empty()) return -1;
            const int id = eSoundsBase::sSounds.id(soundStr);
            if(id < 0) {
                eRuntimeThrow("Unrecognized sound \"" + soundStr + "\".");
            }
            return id;
        };

        missile.mAppearSoundId = parseSound("appearSound");
        missile.mHitSoundId = parseSound("hitSound");
        missile.mLoopSoundId = parseSound("loopSound");
        missile.mDisappearSoundId = parseSound("disappearSound");

        const int id = sMissiles.add(name, missile);
        if(name == "flesh") {
            sFleshId = id;
        } else if(name == "ice") {
            sIceId = id;
        }
    }

    for(const auto& it : eSkills::sSkills) {
        auto& skill = it.fValue;
        skill.fMissileId = sMissiles.id(skill.fMissileStr);
        skill.fAreaMissileId = sMissiles.id(skill.fAreaMissileStr);
        skill.fSelfAreaMissileId = sMissiles.id(skill.fSelfAreaMissileStr);
    }

    for(const auto& it : eItemsData::sItems) {
        auto& item = it.fValue;
        item.fMissileId = sMissiles.id(item.fMissileStr);
    }
}
