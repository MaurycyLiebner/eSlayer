#ifndef EITEMSDATA_H
#define EITEMSDATA_H

#include "eSlayerHelpers/eitemdata.h"
#include "eSlayerHelpers/estringidmapvector.h"

class ESLAYERHELPERS_API eItemsData {
public:
    static eItemData& get(const std::string& name);
    static eItemData& get(const int id);
    static int id(const std::string& name);
    static std::string name(const int id);
    static void load();

    static std::vector<int> sGoldIds;
    static eStringIdMapVector<eItemData> sItems;
private:
    static void load(const std::string& name,
                     const eItemType type);

    static bool sLoaded;
};

#endif // EITEMSDATA_H
