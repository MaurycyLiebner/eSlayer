#ifndef ESKILLBUTTON_H
#define ESKILLBUTTON_H

#include "../ebuttonbase.h"

class eSkillButton : public eButtonBase {
public:
    using eButtonBase::eButtonBase;

    void initialize();

    void setSkillId(const int skillId);
    int skillId() { return mSkillId; }
private:
    int mSkillId = 0;
};

#endif // ESKILLBUTTON_H
