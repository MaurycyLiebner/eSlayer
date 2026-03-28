#ifndef ECHARACTER_H
#define ECHARACTER_H

#include <eSlayerHelpers/eequipment.h>

#include <string>

class eCharacter {
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

    const eEquipment& equipment() const { return mEquipment; }
private:
    std::string mName;
    bool mHardcore;
    bool mDead = false;
    eEquipment mEquipment;
};

#endif // ECHARACTER_H
