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
private:
    void loadImpl();
    eItemData& getImpl(const std::string& name);
    eItemData& getImpl(const int id);
    int idImpl(const std::string& name) const;
    std::string nameImpl(const int id) const;

    static eItemsData sInstance;

    bool mLoaded = false;
    eStringIdMapVector<eItemData> mItems;
};

#endif // EITEMSDATA_H
