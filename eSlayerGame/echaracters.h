#ifndef ECHARACTERS_H
#define ECHARACTERS_H

#include "eSlayerHelpers/echaracter.h"

#include <vector>

class eCharacters {
public:
    eCharacters();

    bool empty() const { return mCharacters.empty(); }
    bool contains(const std::string& name) const;
    bool add(const std::string& name,
             const bool hardcore);
    bool remove(const std::string& name);
    eCharacter get(const std::string& name) const;
    const std::vector<eCharacter>& get() const { return mCharacters; }

    void load();
private:
    std::vector<eCharacter> mCharacters;
};

#endif // ECHARACTERS_H
