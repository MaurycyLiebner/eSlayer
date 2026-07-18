#ifndef ECHARACTER_H
#define ECHARACTER_H

#include "eslayerhelpersexport.h"

#include "eSlayerHelpers/eequipment.h"
#include "eSlayerHelpers/eattributes.h"
#include "eSlayerHelpers/estats.h"
#include "eSlayerHelpers/ewaypoints.h"
#include "eSlayerHelpers/eslayerquests.h"
#include "eSlayerHelpers/etalkheard.h"
#include "eSlayerHelpers/emercenary.h"

#include <string>

class ESLAYERHELPERS_API eCharacter {
public:
    eCharacter() {}
    eCharacter(const int classId,
               const std::string& name,
               const bool hardcore);

    static bool load(const std::string& path,
                     eCharacter& c);
    bool write(const std::string& path) const;

    const std::string& name() const { return mName; }
    bool hardcore() const { return mHardcore; }

    bool running() const { return mRunning; }
    void setRunning(const bool r) { mRunning = r; }

    const eEquipment& equipment() const { return mEquipment; }
    const eAttributes& attributes() const { return mAttributes; }

    eEquipment& equipment() { return mEquipment; }
    eAttributes& attributes() { return mAttributes; }

    const eSlayerQuests& quests(const int diff) const
    { return mQuests[diff]; }
    eSlayerQuests& quests(const int diff)
    { return mQuests[diff]; }

    const eTalkHeard& talkHeard(const int diff) const
    { return mTalkHeard[diff]; }
    eTalkHeard& talkHeard(const int diff)
    { return mTalkHeard[diff]; }

    const eSkillLevels& skillLevels() const { return mSkillLevels; }
    eSkillLevels& skillLevels() { return mSkillLevels; }

    const std::map<int, int>& leftHotkeys() const { return mLeftHotkeys; }
    std::map<int, int>& leftHotkeys() { return mLeftHotkeys; }

    const std::map<int, int>& rightHotkeys() const { return mRightHotkeys; }
    std::map<int, int>& rightHotkeys() { return mRightHotkeys; }

    const int& leftSkill() const { return mLeftSkill; }
    int& leftSkill() { return mLeftSkill; }

    const int& rightSkill() const { return mRightSkill; }
    int& rightSkill() { return mRightSkill; }

    const int& otherLeftSkill() const { return mOtherLeftSkill; }
    int& otherLeftSkill() { return mOtherLeftSkill; }

    const int& otherRightSkill() const { return mOtherRightSkill; }
    int& otherRightSkill() { return mOtherRightSkill; }

    const std::vector<eBodyEquipment>& bodies() const { return mBodies; }
    std::vector<eBodyEquipment>& bodies() { return mBodies;}

    const eWaypoints& waypoints(const int diff) const
    { return mWaypoints[diff]; }
    eWaypoints& waypoints(const int diff)
    { return mWaypoints[diff];}

    const std::optional<eMercenary>& merc() const { return mMerc; }
    std::optional<eMercenary>& merc() { return mMerc;}

    const int latestDifficulty() const { return mLatestDifficulty; }
    int& latestDifficulty() { return mLatestDifficulty; }

    const int classId() const { return mClassId; }

    void read(ePacket& p);
    void write(ePacket& p) const;
private:
    int mClassId;
    std::string mName;
    bool mHardcore;
    eEquipment mEquipment;
    eAttributes mAttributes;
    eSkillLevels mSkillLevels;
    std::vector<eBodyEquipment> mBodies;
    int mLeftSkill = 0;
    int mRightSkill = 0;
    int mOtherLeftSkill = 0;
    int mOtherRightSkill = 0;
    std::map<int, int> mLeftHotkeys;
    std::map<int, int> mRightHotkeys;
    std::vector<eWaypoints> mWaypoints;
    std::vector<eSlayerQuests> mQuests;
    std::vector<eTalkHeard> mTalkHeard;
    std::optional<eMercenary> mMerc;
    int mLatestDifficulty = 0;
    bool mRunning = false;
};

#endif // ECHARACTER_H
