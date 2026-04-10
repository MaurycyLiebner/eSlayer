#ifndef ESKILLNAMES_H
#define ESKILLNAMES_H

#include <map>
#include <string>

class eSkillNames {
public:
    static std::string name(const int skillId);
    static std::string description(const int skillId);
    static bool load();
private:
    static eSkillNames sInstance;

    bool mLoaded = false;
    std::map<int, std::string> mNames;
    std::map<int, std::string> mDescriptions;
};

#endif // ESKILLNAMES_H
