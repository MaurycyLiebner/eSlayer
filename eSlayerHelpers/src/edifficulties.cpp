#include "eSlayerHelpers/edifficulties.h"

#include "eSlayerHelpers/efileloaderbase.h"

eStringIdMapVector<int> eDifficulties::sDifficulties;
int eDifficulties::sDifficulty = -1;
bool eDifficulties::sFinished = false;
bool eDifficulties::sLoaded = false;

void eDifficulties::load() {
    if(sLoaded) return;
    sLoaded = true;

    const auto dir = "Other";

    try {
        const auto jdata = eFileLoaderBase::parse(dir, "difficulties.json");
        const auto difficulties = jdata.get<std::vector<std::string>>();

        for(const auto& d : difficulties) {
            sDifficulties.add(d, 0);
        }
    } catch(...) {
        eRuntimeThrow("Error while parsing Other/difficulties.json");
    }
}
