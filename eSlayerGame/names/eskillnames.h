#ifndef ESKILLNAMES_H
#define ESKILLNAMES_H

#include <map>
#include <string>

class eSkillNames {
public:
    static std::string name(const int skillId);
    static bool load();
private:
    static eSkillNames sInstance;

    bool mLoaded = false;
    std::map<int, std::string> mNames;
};

#endif // ESKILLNAMES_H
