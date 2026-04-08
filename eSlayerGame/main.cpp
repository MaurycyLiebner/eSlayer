#include "audio/emusic.h"
#include "audio/esounds.h"
#include "emainwindow.h"
#include "eresolution.h"
#include "ewindowsettings.h"
#include "elanguage.h"
#include "screens/escreenhandler.h"
#include "textures/echarstextures.h"
#include "textures/eterrstextures.h"
#include "textures/eobjstextures.h"
#include "textures/eeffectstextures.h"
#include "textures/emissilestextures.h"
#include "textures/eitemstextures.h"

#include "names/eitemnames.h"
#include "names/emonsternames.h"
#include "names/eskillnames.h"
#include "names/eskilltreenames.h"

#include <eSlayerHelpers/eexceptions.h>
#include <eSlayerHelpers/erunsettings.h>
#include <eSlayerHelpers/eskills.h>
#include <eSlayerHelpers/echardatainfo.h>
#include <eSlayerHelpers/egamedir.h>
#include <eSlayerHelpers/eitemsdata.h>
#include <eSlayerHelpers/eattributes.h>
#include <eSlayerHelpers/eterrstexturesdata.h>
#include <eSlayerHelpers/eobjstexturesdata.h>
#include <eSlayerHelpers/eunitsinfo.h>
#include <eSlayerHelpers/eskilltrees.h>
#include <eSlayerHelpers/eclasses.h>

#include <eSlayerMissiles/emissileincrement.h>

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3/SDL_main.h>

#include <vector>

bool init() {
    if(!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        eExceptions::logError(
            "SDL could not initialize!",
            SDL_GetError());
        return false;
    }

    if(!MIX_Init()) {
        eExceptions::logError(
            "SDL_mixer could not initialize!",
            SDL_GetError());
        return false;
    }

    if(!TTF_Init()) {
        eExceptions::logError(
            "SDL_ttf could not initialize!",
            SDL_GetError());
        return false;
    }

    return true;
}

void close() {
    TTF_Quit();
    MIX_Quit();
    SDL_Quit();
}

bool getDisplayResolutions(std::vector<SDL_DisplayMode>& resolutions) {
    int displayCount;
    const auto displays = SDL_GetDisplays(&displayCount);

    SDL_Log("Number of displays: %i", displayCount);

    for(int iDisplay = 0; iDisplay < displayCount; iDisplay++) {
        SDL_Log("Display %i:", iDisplay);

        const auto displayId = displays[iDisplay];
        int modesCount;
        const auto modes = SDL_GetFullscreenDisplayModes(displayId, &modesCount);

        for(int jMode = 0; jMode < modesCount; jMode++) {
            const auto mode = modes[jMode];
            resolutions.push_back(*mode);
        }
    }
    return true;
}

std::string strToUpper(std::string s) {
    const auto transform = [](const unsigned char c) {
        return std::toupper(c);
    };
    std::transform(s.begin(), s.end(), s.begin(), transform);
    return s;
}

int main(int argc, char* argv[]) {
    eGameDir::sExePath = SDL_GetBasePath();
    for(int i = 1; i < argc - 1; i += 2) {
        const std::string arg = argv[i];
        std::string value = argv[i + 1];
        value = strToUpper(value);
        if(arg == "--zip") {
            eRunSettings::sUseZip = value == "TRUE";
        } else if(arg == "--fps") {
            eRunSettings::sFPS = std::stof(value);
        }
    }

    SDL_SetLogOutputFunction([](void *userdata, int category, SDL_LogPriority priority, const char *message) {
        const auto def = SDL_GetDefaultLogOutputFunction();
        def(userdata, category, priority, message);
        if(category == SDL_LOG_CATEGORY_ERROR) {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", message, nullptr);
        }
    }, nullptr);

    eExceptions::setLogger([](const std::string& msg) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "%s", msg.c_str());
    });

    eExceptions::setDialogShower([](const std::string& msg) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", msg.c_str(), nullptr);
    });

    if(!init()) {
        eExceptions::logError("Failed to initialize!");
        close();
        return 1;
    }

    std::vector<SDL_DisplayMode> resolutions;
    getDisplayResolutions(resolutions);

    for(const auto& r : resolutions) {
        bool contains = false;
        for(const auto& rr : eResolution::sResolutions) {
            contains = r.w == rr.width() &&
                       r.h == rr.height();
            if(contains) break;
        }
        if(!contains) {
            const auto res = eResolution(r.w, r.h);
            eResolution::sResolutions.push_back(res);
        }
    }

    std::sort(eResolution::sResolutions.begin(),
              eResolution::sResolutions.end());

    const auto mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
    if(!mixer) {
        eExceptions::logError(
            "SDL_mixer could not create mixer!",
            SDL_GetError());
        close();
        return 1;
    }

    eWindowSettings settings;
    {
        const bool r = settings.read();
        if(!r) settings.write();
    }

    int r = 0;
    try {
        eMainWindow w;
        eScreenHandler sh(&w);
        const bool i = w.initialize(settings);
        if(!i) return 1;

        eMusic music(mixer);
        const bool m = music.initialize();
        if(!m) return 1;
        eSounds sounds(mixer);

        const auto showMainMenu = [&]() {
            sh.showMainMenu();
        };

        std::vector<eAction> loadings;

        loadings.emplace_back([&]() {
            eMusic::loadMenu();
            music.playMenuMusic();
        });

        loadings.emplace_back([&]() {
            eSounds::loadButtonSound();
        });

        loadings.emplace_back([&]() {
            eLanguage::load();
        });

        loadings.emplace_back([&]() {
            eSkills::load();
        });

        loadings.emplace_back([&]() {
            eSkillTrees::load();
        });

        loadings.emplace_back([&]() {
            eSkillTreeNames::load();
        });

        loadings.emplace_back([&]() {
            eClasses::load();
        });

        loadings.emplace_back([&]() {
            eCharDataInfo::load();
        });

        loadings.emplace_back([&]() {
            eCharsTextures::load();
        });

        loadings.emplace_back([&]() {
            eUnitsInfo::load();
        });

        loadings.emplace_back([&]() {
            eItemsData::load();
        });

        loadings.emplace_back([&]() {
            eItemsTextures::load();
        });

        loadings.emplace_back([&]() {
            eItemNames::load();
        });

        loadings.emplace_back([&]() {
            eMonsterNames::load();
        });

        loadings.emplace_back([&]() {
            eSkillNames::load();
        });

        loadings.emplace_back([&]() {
            eTerrsTexturesData::load();
            eTerrsTextures::load();
        });

        loadings.emplace_back([&]() {
            eObjsTexturesData::load();
            eObjsTextures::load();
        });

        loadings.emplace_back([&]() {
            eEffectsTextures::load();
        });

        loadings.emplace_back([&]() {
            eMissilesTextures::loadData();
            eMissileIncrement::initialize();
        });

        loadings.emplace_back([&]() {
            eAttributes::load();
        });

        sh.showLoadingScreen(loadings, showMainMenu);

        r = w.exec();
    } catch(const std::exception& e) {
        eExceptions::showDialog(e);
    }

    close();

    return r;
}
