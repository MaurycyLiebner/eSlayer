#include "eSlayerHelpers/echardata.h"

#include "eSlayerHelpers/epacket.h"

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
    {"cast", &eCharData::mCastId},
    {"hitRecovery", &eCharData::mHitRecoveryId},
    {"death", &eCharData::mDeathId},
    {"body", &eCharData::mBodyId},
};

eCharData::eCharData() {}

void eCharData::load(ordered_json& jdata) {
    mDirs = jdata["directions"];
    mRadius = jdata["radius"];
    const auto& anims = jdata["animations"];
    for(auto& [name, animData] : anims.items()) {
        const auto offset = animData.value("offset", std::vector<int>{0, 0});
        eAnimation anim;
        anim.fFrames = animData.value("frames", 0);
        anim.fOffset = eOffset{offset[0], offset[1]};
        anim.fClamp = animData.value("clamp", "");
        anim.fActionFrame = animData.value("actionFrame", anim.fFrames);
        const int id = mAnims.add(name, anim);
        setAnimId(name, id);
    }

    for(const auto& it : mAnims) {
        auto& anim = it.fValue;
        anim.fClampId = animId(anim.fClamp);
    }

    const auto groups = jdata["groups"];
    for(const auto& groupJson : groups) {
        auto& group = mGroups.emplace_back();
        for(auto& [key, valueArray] : groupJson.items()) {
            const auto values = valueArray.get<std::vector<std::string>>();
            eStringIdMapVector<bool> equipment;
            for(const auto& v : values) {
                equipment.add(v, true);
            }
            const int id = mParts.add(key, equipment);
            group.emplace_back(id);
            mNParts++;
        }
    }

    for(const auto& it : eSkills::sSkills) {
        auto& uskill = mSkills.emplace_back();
        uskill.fSkillId = it.fId;
        auto& skill = it.fValue;
        for(const auto& a : skill.fCastAnims) {
            const int aid = animId(a);
            if(aid == -1) continue;
            uskill.fCastAnimIds.push_back(aid);
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
    return mAnims.get(id).fFrames;
}

int eCharData::animFrames(const std::string& name) const {
    const int id = animId(name);
    return animFrames(id);
}

int eCharData::animActionFrame(const int id) const {
    return mAnims.get(id).fActionFrame;
}

int eCharData::animActionFrame(const std::string& name) const {
    const int id = animId(name);
    return animFrames(id);
}

int eCharData::animClamp(const int id) const {
    return mAnims.get(id).fClampId;
}

int eCharData::animClamp(const std::string& name) const {
    const int id = animId(name);
    return animClamp(id);
}

eModelParts eCharData::mapToModelParts(
    const std::map<std::string, std::string>& m) const {
    if(m.size() != mNParts) {
        eRuntimeThrow("Insufficient unit equipment information.");
    }
    eModelParts result;
    result.fValues.resize(mNParts);
    for(const auto& part : m) {
        const auto& partName = part.first;
        const int partId = mParts.id(partName);
        if(partId == -1) {
            eRuntimeThrow("Part \"" + partName + "\" not found.");
        }
        const auto& partData = mParts.get(partId);
        const auto& eqName = part.second;
        const int eqId = partData.id(eqName);
        if(eqId == -1) {
            eRuntimeThrow("Part \"" + partName + "\" does not have \"" + eqName + "\".");
        }
        result.fValues[partId] = eqId;
    }

    return result;
}

const eUnitSkill& eCharData::getSkill(const int id) const {
    return mSkills[id];
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

void eModelParts::read(ePacket& p) {
    uint8_t nVals;
    p >> nVals;
    fValues.reserve(nVals);
    for(uint8_t i = 0; i < nVals; i++) {
        uint8_t val;
        p >> val;
        fValues.emplace_back(val);
    }
}

void eModelParts::write(ePacket& p) const {
    const uint8_t nVals = fValues.size();
    p << nVals;
    for(const uint8_t val : fValues) {
        p << val;
    }
}
