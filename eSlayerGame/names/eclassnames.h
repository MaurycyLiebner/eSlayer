#ifndef ECLASSNAMES_H
#define ECLASSNAMES_H

#include <map>
#include <string>

class eClassNames {
public:
    static std::string name(const int classId);
    static bool load();
    static bool reload();
private:
    static eClassNames sInstance;

    bool mLoaded = false;
    std::map<int, std::string> mNames;
};

#endif // ECLASSNAMES_H
