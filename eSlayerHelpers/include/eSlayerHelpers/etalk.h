#ifndef ETALK_H
#define ETALK_H

#include "estringidmapvector.h"

enum class eConvoType {
    intro,
    questIntro, questOutro,
    questStep
};

struct eConvo {
    std::string fName;
    eConvoType fType;
    int fQuestId;
};

struct eTalk {
    std::vector<eConvo> fConvo;
};

class eTalks {
public:
    static eStringIdMapVector<eTalk> sTalk;

    static void load();
private:
    static bool sLoaded;
};

#endif // ETALK_H
