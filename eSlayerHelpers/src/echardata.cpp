#include "eSlayerHelpers/echardata.h"

#include "eSlayerHelpers/epacket.h"
#include "eSlayerHelpers/erand.h"

eCharData::eCharData() {}

void eCharData::load(const std::string& name,
                     const ordered_json& jdata) {
    mDirs = jdata["directions"];
    const auto& anims = jdata["animations"];
    for(const auto& [name, animData] : anims.items()) {
        const auto overwrite = animData.value("overwrite", "");
        if(overwrite.empty()) {
            eAnimation anim;
            anim.fFrames = animData.value("frames", 0);
            anim.fClamp = animData.value("clamp", "");
            anim.fActionFrame = animData.value("actionFrame", anim.fFrames);
            const int id = mAnims.add(name, anim);
            setAnimId(name, id);
        } else {
            const int id = animId(overwrite);
            setAnimId(name, id);
        }
    }

    for(const auto& it : mAnims) {
        auto& anim = it.fValue;
        anim.fClampId = animId(anim.fClamp);
    }

    if(jdata.contains("groups")) {
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
    } else {
        auto& group = mGroups.emplace_back();
        eStringIdMapVector<bool> equipment;
        equipment.add("whole", true);
        const int id = mParts.add(name, equipment);
        group.emplace_back(id);
        mNParts++;
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
    const auto it = mAnimIds.find(name);
    if(it == mAnimIds.end()) return -1;
    return it->second;
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
    eModelParts result;
    result.fValues.resize(mNParts, 255);
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

eModelParts eCharData::randomModelParts() const {
    eModelParts result;
    result.fValues.resize(mNParts, 255);
    for(const auto& part : mParts) {
        const auto& partName = part.fName;
        const int partId = part.fId;
        const auto& partData = mParts.get(partId);
        const auto nEq = partData.size();
        if(nEq == 0) continue;
        const int eqId = eRand::rand() % nEq;
        result.fValues[partId] = eqId;
    }
    return result;
}

const eUnitSkill& eCharData::getSkill(const int id) const {
    return mSkills[id];
}

void eCharData::setAnimId(const std::string& name, const int id) {
    if(name == "run") mRunId = id;
    else if(name == "hitRecovery") mHitRecoveryId = id;
    else if(name == "block") mBlockId = id;
    else if(name == "death") mDeathId = id;
    else if(name == "body") mBodyId = id;
    else if(name == "raise") mRaiseId = id;
    mAnimIds[name] = id;
}

void eModelParts::read(ePacket& p) {
    p.read8(fValues);
}

void eModelParts::write(ePacket& p) const {
    p.write8(fValues);
}
