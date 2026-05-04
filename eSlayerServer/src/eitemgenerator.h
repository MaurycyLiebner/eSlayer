#ifndef EITEMGENERATOR_H
#define EITEMGENERATOR_H

#include <cstdint>

struct eItem;

class eItemGenerator {
public:
    static void applyItemId(eItem& item);
    static eItem generateItem(
        const float level, const float worth);
private:
    static uint32_t sNextItemId;
};

#endif // EITEMGENERATOR_H
