#ifndef EITEMNAMES_H
#define EITEMNAMES_H

#include <map>
#include <string>

struct eItemBase;

class eItemNames {
public:
    static std::string name(const eItemBase& item);
    static const std::string& name(const int itemDataId);

    static const std::string& prefixName(const int id);
    static const std::string& suffixName(const int id);

    static const std::string& weaponClassName(const int id);

    static bool load();
    static bool reload();
private:
    static eItemNames sInstance;

    bool mLoaded = false;
    std::map<int, std::string> mNames;
    std::map<int, std::string> mPrefixNames;
    std::map<int, std::string> mSuffixNames;
    std::map<int, std::string> mWeaponClassNames;
};

#endif // EITEMNAMES_H
