#ifndef EQUESTS_H
#define EQUESTS_H

#include "estringidmapvector.h"

enum class eQuestType {
    kill, talkTo
};

struct eQuestStep {
    eQuestType fType;
    int fTarget = 0;
    int fCount = 0;
};

struct eQuest {
    std::string fTexture;
    int fAct = 0;
    std::vector<eQuestStep> fSteps;
    std::vector<int> fPrerequisites;
};

class eQuests {
public:
    static eStringIdMapVector<eQuest> sQuests;

    static void load();
private:
    static bool sLoaded;
};

#endif // EQUESTS_H
