#ifndef ESKILLTREES_H
#define ESKILLTREES_H

#include "eskilltree.h"

#include "estringidmapvector.h"

class ESLAYERHELPERS_API eSkillTrees {
public:
    static void load();

    static eStringIdMapVector<eSkillTree> sTrees;
private:
    static bool sLoaded;
};

#endif // ESKILLTREES_H
