#ifndef EITEMPARTSMAP_H
#define EITEMPARTSMAP_H

#include "eslayerhelpersexport.h"

#include <map>
#include <string>
#include <vector>

enum class eItemPlace {
    weaponR,
    weaponL,
    helmet,
    armor
};

using eStrMap = std::map<std::string, std::string>;
using eItemStrMap = std::map<int, eStrMap>;

struct eItemPlaceItem {
    eItemPlace fPlace;
    int fItemId;
};

class ESLAYERHELPERS_API eItemPartsMap {
public:
    static void load();

    static eStrMap get(const std::vector<eItemPlaceItem> items);
    static std::map<eItemPlace, eItemStrMap> sMap;
    static eStrMap sPartsBase;
private:
    static const eStrMap& get(const eItemPlaceItem item);

    static bool sLoaded;
};

#endif // EITEMPARTSMAP_H
