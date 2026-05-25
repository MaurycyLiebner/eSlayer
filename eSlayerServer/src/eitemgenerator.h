#ifndef EITEMGENERATOR_H
#define EITEMGENERATOR_H

#include <cstdint>

struct eItem;

class eItemGenerator {
public:
    static void applyItemId(eItem& item);
    static eItem generatePotion(
        const int level, const float worth);
    static eItem generateItem(
        const int level, const float worth);
private:
    static uint32_t sNextItemId;
};

#endif // EITEMGENERATOR_H
