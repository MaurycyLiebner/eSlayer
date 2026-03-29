#ifndef EITEMNAMES_H
#define EITEMNAMES_H

#include <map>
#include <string>

class eItemNames {
public:
    static std::string name(const int itemDataId);
    static bool load();
private:
    static eItemNames sInstance;

    bool mLoaded = false;
    std::map<int, std::string> mNames;
};

#endif // EITEMNAMES_H
