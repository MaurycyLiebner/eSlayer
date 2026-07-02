#ifndef EQUESTS_H
#define EQUESTS_H

#include "estringidmapvector.h"

enum class eQuestType {
    kill, talkTo
};

struct eQuestStep {
    std::string fConvoStr;
    eQuestType fType;
    int fTarget = 0;
    int fCount = 0;
};

struct ESLAYERHELPERS_API eQuest {
    int fAct = 0;
    std::vector<eQuestStep> fSteps;
    std::vector<int> fPrerequisites;

    int nStages() const {
        return fSteps.size() + 2;
    }
};

class ESLAYERHELPERS_API eQuests {
public:
    static eStringIdMapVector<eQuest> sQuests;

    static void load();
private:
    static bool sLoaded;
};

#endif // EQUESTS_H
