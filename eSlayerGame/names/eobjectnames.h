#ifndef EOBJECTNAMES_H
#define EOBJECTNAMES_H

#include <map>
#include <string>

class eObjectNames {
public:
    static std::string name(const int objectId);
    static bool load();
    static bool reload();
private:
    static eObjectNames sInstance;

    bool mLoaded = false;
    std::map<int, std::string> mNames;
};

#endif // EOBJECTNAMES_H
