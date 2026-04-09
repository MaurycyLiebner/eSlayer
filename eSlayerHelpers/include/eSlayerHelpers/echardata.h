#ifndef ECHARDATA_H
#define ECHARDATA_H

#include "eoffset.h"

#include "estringidmapvector.h"
#include "eskills.h"

#include <map>
#include <string>

#include <nlohmann/json.hpp>
using namespace nlohmann;

class ePacket;

struct eModelParts {
    std::vector<uint8_t> fValues;

    void read(ePacket& p);
    void write(ePacket& p) const;
};

class ESLAYERHELPERS_API eCharData {
    friend class eCharTextures;
public:
    eCharData();

    virtual void load(const ordered_json& jdata);

    void setTypeId(const int id) { mId = id; }
    int typeId() const { return mId; }

    void setName(const std::string& name) { mName = name; }
    const std::string& name() const { return mName; }

    int animId(const std::string& name) const;
    int runAnimId() const { return mRunId; }
    int hitRecoveryAnimId() const { return mHitRecoveryId; }
    int blockAnimId() const { return mBlockId; }

    int animFrames(const int id) const;
    int animFrames(const std::string& name) const;

    int animActionFrame(const int id) const;
    int animActionFrame(const std::string& name) const;

    int animClamp(const int id) const;
    int animClamp(const std::string& name) const;

    eModelParts mapToModelParts(const std::map<std::string, std::string>& m) const;

    const eUnitSkill& getSkill(const int id) const;
protected:
    void setAnimId(const std::string& name, const int id);

    std::string mName;
    int mId;
    int mDirs;

    int mRunId = -1;
    int mHitRecoveryId = -1;
    int mBlockId = -1;
    std::map<std::string, int> mAnimIds;

    struct eAnimation {
        int fFrames;
        eOffset fOffset;
        std::string fClamp;
        int fClampId;
        int fActionFrame;
    };

    eStringIdMapVector<eAnimation> mAnims;
    std::vector<std::vector<int>> mGroups;
    eStringIdMapVector<eStringIdMapVector<bool>> mParts;
    int mNParts = 0;

    std::vector<eUnitSkill> mSkills;
};

#endif // ECHARDATA_H
