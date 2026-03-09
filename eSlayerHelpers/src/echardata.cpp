#include "eSlayerHelpers/echardata.h"

const std::unordered_map<std::string, int eCharData::*>
eCharData::sAnimFields = {
    {"stand", &eCharData::mStandId},
    {"standReady", &eCharData::mStandReadyId},
    {"walk", &eCharData::mWalkId},
    {"walkReady", &eCharData::mWalkReadyId},
    {"run", &eCharData::mRunId},
    {"attack1", &eCharData::mAttack1Id},
    {"attack2", &eCharData::mAttack2Id},
    {"block", &eCharData::mBlockId},
    {"getHit", &eCharData::mGetHitId},
    {"death", &eCharData::mDeathId},
    {"body", &eCharData::mBodyId},
};

eCharData::eCharData() {}

void eCharData::load(ordered_json& jdata) {
    mDirs = jdata["directions"];
    mRadius = jdata["radius"];
    const auto anims = jdata["animations"].items();
    int id = 0;
    for(auto& [name, animData] : anims) {
        setAnimId(name, id++);
        const auto offset = animData.value("offset", std::vector<int>{0, 0});
        auto& animP = mAnims.emplace_back();
        animP.first = name;
        auto& anim = animP.second;
        anim.fFrames = animData.value("frames", 0);
        anim.fOffset = eOffset{offset[0], offset[1]};
        anim.fClamp = animData.value("clamp", false);
    }

    const auto groups = jdata["groups"];
    for(const auto& groupJson : groups) {
        auto& group = mGroups.emplace_back();
        const auto items = groupJson.items();
        for(auto& [key, valueArray] : items) {
            const auto values = valueArray.get<std::vector<std::string>>();
            group[key] = values;
        }
    }
}

int eCharData::animId(const std::string& name) const {
    {
        const auto it = sAnimFields.find(name);
        if(it != sAnimFields.end()) {
            return this->*(it->second);
        }
    }

    {
        const auto it = mCustomIds.find(name);
        if(it == mCustomIds.end()) return -1;
        return it->second;
    }
}

int eCharData::animFrames(const int id) const {
    return mAnims[id].second.fFrames;
}

int eCharData::animFrames(const std::string& name) const {
    const int id = animId(name);
    return animFrames(id);
}

bool eCharData::animClamp(const int id) const {
    return mAnims[id].second.fClamp;
}

bool eCharData::animClamp(const std::string& name) const {
    const int id = animId(name);
    return animClamp(id);
}

void eCharData::setAnimId(const std::string& name, const int id) {
    {
        const auto it = sAnimFields.find(name);
        if(it != sAnimFields.end()) {
            this->*(it->second) = id;
            return;
        }
    }

    mCustomIds[name] = id;
}
