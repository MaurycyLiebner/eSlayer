#include "eSlayerHelpers/edifficulties.h"

#include "eSlayerHelpers/efileloaderbase.h"

eStringIdMapVector<eDifficulty>
eDifficulties::sDifficulties;
int eDifficulties::sDifficulty = -1;
bool eDifficulties::sLoaded = false;

void eDifficulties::load() {
    if(sLoaded) return;
    sLoaded = true;

    const auto dir = "Other";

    try {
        const auto jdata = eFileLoaderBase::parse(dir, "difficulties.json");

        for(const auto& [name, jDiff] : jdata.items()) {
            eDifficulty diff;
            diff.fResistPenalty = jDiff.value("resistPenalty", 0.f);
            diff.fLeechPenalty = jDiff.value("leechPenalty", 0.f);
            diff.fColdLengthPenalty = jDiff.value("coldLengthPenalty", 0.f);
            diff.fFreezeLengthPenalty = jDiff.value("freezeLengthPenalty", 0.f);
            sDifficulties.add(name, diff);
        }
    } catch(...) {
        eRuntimeThrow("Error while parsing Other/difficulties.json");
    }
}
