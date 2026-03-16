#ifndef ECHARDATA_H
#define ECHARDATA_H

#include "eoffset.h"

#include <map>
#include <string>

#include <nlohmann/json.hpp>
using namespace nlohmann;

class ePacket;

using eModelParts = std::map<std::string, std::string>;
struct eCompressedModelParts {
    std::vector<uint8_t> fValues;

    void read(ePacket& p);
    void write(ePacket& p) const;
};

class eCharData {
public:
    eCharData();

    virtual void load(ordered_json& jdata);

    void setTypeId(const int id) { mId = id; }
    int typeId() const { return mId; }

    void setName(const std::string& name) { mName = name; }
    const std::string& name() const { return mName; }

    double radius() const { return mRadius; }

    int animId(const std::string& name) const;

    int animFrames(const int id) const;
    int animFrames(const std::string& name) const;

    int animActionFrame(const int id) const;
    int animActionFrame(const std::string& name) const;

    const std::string& animClamp(const int id) const;
    const std::string& animClamp(const std::string& name) const;

    eCompressedModelParts
    compress(const eModelParts& parts);

    eModelParts
    decompress(const eCompressedModelParts& parts);
protected:
    void setAnimId(const std::string& name, const int id);

    std::string mName;
    int mId;
    int mDirs;

    double mRadius;

    static const std::unordered_map<std::string, int eCharData::*>
    sAnimFields;
    int mStandId = -1;
    int mStandReadyId = -1;
    int mWalkId = -1;
    int mWalkReadyId = -1;
    int mRunId = -1;
    int mAttack1Id = -1;
    int mAttack2Id = -1;
    int mBlockId = -1;
    int mHitRecoveryId = -1;
    int mDeathId = -1;
    int mBodyId = -1;
    std::map<std::string, int> mCustomIds;

    struct eAnimation {
        int fFrames;
        eOffset fOffset;
        std::string fClamp;
        int fActionFrame;
    };

    std::vector<std::pair<std::string, eAnimation>> mAnims;
    std::vector<std::map<std::string, std::vector<std::string>>> mGroups;
};

#endif // ECHARDATA_H
