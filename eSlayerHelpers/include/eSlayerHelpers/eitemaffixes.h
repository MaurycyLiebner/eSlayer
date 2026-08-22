#ifndef EITEMAFFIXES_H
#define EITEMAFFIXES_H

#include "eslayerhelpersexport.h"

#include "ecolor.h"
#include "emodscollection.h"
#include "estringidmapvector.h"

#include <set>

struct ESLAYERHELPERS_API eItemAffix {
    int fLevelReq;
    eModsCollectionLevels fLevels;
    eColor fColor;

    void generate(const int level, float& remWorth,
                  std::vector<eModifier>& mods) const;
};

class ESLAYERHELPERS_API eItemAffixes {
public:
    static void load();

    static const int sMaxItemLevel;
    static std::map<int, std::set<int>> sTypePrefixes;
    static eStringIdMapVector<eItemAffix> sPrefixes;
    static std::map<int, std::set<int>> sTypeSuffixes;
    static eStringIdMapVector<eItemAffix> sSuffixes;
private:
    static bool sLoaded;
};

#endif // EITEMAFFIXES_H
