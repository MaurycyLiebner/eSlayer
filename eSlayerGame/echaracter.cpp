#include "echaracter.h"

#include <eSlayerHelpers/eitemsdata.h>

#include <algorithm>
#include <tinyxml2.h>
using namespace tinyxml2;

eCharacter::eCharacter(const std::string& name,
                       const bool hardcore) :
    mName(name), mHardcore(hardcore) {}

bool isTrue(std::string value) {
    std::transform(value.begin(), value.end(),
                   value.begin(), ::tolower);
    return value == "true";
}

bool eCharacter::load(const std::string& path,
                      eCharacter& c) {
    XMLDocument doc;
    const auto e = doc.LoadFile(path.c_str());
    if(e) {
        printf("Character XML LoadFile error %i reading %s.\n",
               e, path.c_str());
        return false;
    }
    const auto rootE = doc.RootElement();
    if(!rootE) {
        printf("Missing root element in %s.\n",
               path.c_str());
        return false;
    }
    const auto nameE = rootE->FirstChildElement("name");
    if(!nameE) {
        printf("Missing name element in %s.\n",
               path.c_str());
        return false;
    }
    const auto hardcoreE = rootE->FirstChildElement("hardcore");
    if(!hardcoreE) {
        printf("Missing hardcore element in %s.\n",
               path.c_str());
        return false;
    }
    const auto deadE = rootE->FirstChildElement("dead");
    if(!deadE) {
        printf("Missing dead element in %s.\n",
               path.c_str());
        return false;
    }
    c.mName = nameE->GetText();
    const std::string hardcoreV(hardcoreE->GetText());
    c.mHardcore = isTrue(hardcoreV);
    const std::string deadV(deadE->GetText());
    c.mDead = isTrue(deadV);

    for(int x = 0; x < 5; x++) {
        eItem item{uint8_t(x), 0, eItemType::amulet};
        c.mEquipment.fInventory.push_back(eInventoryItem{item, x, 0, 1, 1});
    }

    const uint8_t armorId = eItemsData::id("armor/quilted_armor");
    eItem armor{5, armorId, eItemType::armor};
    c.mEquipment.fInventory.push_back(eInventoryItem{armor, 0, 1, 2, 3});

    return true;
}

bool eCharacter::write(const std::string& path) const {
    XMLDocument doc;

    const auto decl = doc.NewDeclaration("xml version=\"1.0\" encoding=\"UTF-8\"");
    doc.InsertFirstChild(decl);

    const auto rootE = doc.NewElement("character");
    doc.InsertEndChild(rootE);

    const auto nameE = rootE->InsertNewChildElement("name");
    nameE->SetText(mName.c_str());

    const auto hardcoreE = rootE->InsertNewChildElement("hardcore");
    hardcoreE->SetText(mHardcore ? "true" : "false");

    const auto deadE = rootE->InsertNewChildElement("dead");
    deadE->SetText(mDead ? "true" : "false");

    const auto e = doc.SaveFile(path.c_str());
    if(e) {
        printf("Character XML SaveFile error %i writing %s.\n",
               e, path.c_str());
        return false;
    }
    return true;
}
