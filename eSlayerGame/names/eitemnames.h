#ifndef EITEMNAMES_H
#define EITEMNAMES_H

#include <map>
#include <string>

struct eItemBase;

class eItemNames {
public:
    static std::string name(const eItemBase& item);
    static std::string name(const int itemDataId);

    static std::string prefixName(const int id);
    static std::string suffixName(const int id);

    static bool load();
private:
    static eItemNames sInstance;

    bool mLoaded = false;
    std::map<int, std::string> mNames;
    std::map<int, std::string> mPrefixNames;
    std::map<int, std::string> mSuffixNames;
};

#endif // EITEMNAMES_H
