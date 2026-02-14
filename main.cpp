#include "audio/emusic.h"
#include "audio/esounds.h"
#include "emainwindow.h"
#include "eresolution.h"
#include "ewindowsettings.h"
#include "elanguage.h"
#include "screens/escreenhandler.h"

#include <vector>

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <stdio.h>

bool init() {
    if(!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        printf("SDL could not initialize! SDL Error: %s\n",
               SDL_GetError());
        return false;
    }


    if(!MIX_Init()) {
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n",
               SDL_GetError());
        return false;
    }

    if(!TTF_Init()) {
        printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n",
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

int main(int argc, char* argv[]) {
    if(!init()) {
        printf("Failed to initialize!\n");
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

    const auto mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
    if(!mixer) {
        printf("SDL_mixer could not create mixer! SDL_mixer Error: %s\n",
               SDL_GetError());
        close();
        return 1;
    }

    eWindowSettings settings;
    settings.read();

    int r = 0;
    {
        eMusic music(mixer);
        const bool m = music.initialize();
        if(!m) return 1;
        eMusic::loadMenu();
        music.playMenuMusic();

        eSounds sounds(mixer);
        eSounds::loadButtonSound();

        const bool l = eLanguage::load();
        if(!l) return 1;

        eMainWindow w;
        eScreenHandler sh(&w);
        const bool i = w.initialize(settings);
        if(!i) return 1;
        sh.showMainMenu();
        r = w.exec();
    }

    close();

    return r;
}
