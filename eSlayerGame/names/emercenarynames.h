#ifndef EMERCENARYNAMES_H
#define EMERCENARYNAMES_H

#include <eSlayerHelpers/estringidmapvector.h>

#include <vector>
#include <string>

class eMercenaryNames {
public:
    static eStringIdMapVector<std::vector<std::string>> sNames;
    static std::map<int, std::string> sTypeNames;

    static bool load();
    static bool reload();
private:
    static bool sLoaded;
};

#endif // EMERCENARYNAMES_H
