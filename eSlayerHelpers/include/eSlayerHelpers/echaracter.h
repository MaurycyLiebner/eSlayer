#ifndef ECHARACTER_H
#define ECHARACTER_H

#include "eslayerhelpersexport.h"

#include "eSlayerHelpers/eequipment.h"
#include "eSlayerHelpers/eattributes.h"
#include "eSlayerHelpers/estats.h"

#include <string>

class ESLAYERHELPERS_API eCharacter {
public:
    eCharacter() {}
    eCharacter(const std::string& name,
               const bool hardcore);

    static bool load(const std::string& path,
                     eCharacter& c);
    bool write(const std::string& path) const;

    const std::string& name() const { return mName; }
    bool hardcore() const { return mHardcore; }
    bool dead() const { return mDead; }
    void setDead(const bool d) { mDead = d; }

    const eEquipment& equipment() const { return mEquipment; }
    const eAttributes& attributes() const { return mAttributes; }

    eEquipment& equipment() { return mEquipment; }
    eAttributes& attributes() { return mAttributes; }

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

    void read(ePacket& p);
    void write(ePacket& p) const;
private:
    std::string mName;
    bool mHardcore;
    bool mDead = false;
    eEquipment mEquipment;
    eAttributes mAttributes;
    eSkillLevels mSkillLevels;
    int mLeftSkill = 0;
    int mRightSkill = 0;
    std::map<int, int> mLeftHotkeys;
    std::map<int, int> mRightHotkeys;
};

#endif // ECHARACTER_H
