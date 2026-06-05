#ifndef ESKILLTREENAMES_H
#define ESKILLTREENAMES_H

#include <map>
#include <string>

class eSkillTreeNames {
public:
    static std::string name(const int charId);
    static bool load();
    static bool reload();
private:
    static eSkillTreeNames sInstance;

    bool mLoaded = false;
    std::map<int, std::string> mNames;
};

#endif // ESKILLTREENAMES_H
