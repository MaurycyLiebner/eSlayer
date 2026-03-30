#ifndef EMONSTERNAMES_H
#define EMONSTERNAMES_H

#include <map>
#include <string>

class eMonsterNames {
public:
    static std::string name(const int charId);
    static bool load();
private:
    static eMonsterNames sInstance;

    bool mLoaded = false;
    std::map<int, std::string> mNames;
};

#endif // EMONSTERNAMES_H
