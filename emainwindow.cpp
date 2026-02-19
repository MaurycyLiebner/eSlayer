#include "emainwindow.h"

#include <chrono>
#include <cmath>
#include <thread>

#include "audio/emusic.h"

eMainWindow::eMainWindow() {}

eMainWindow::~eMainWindow() {
    if(mSdlWindow) SDL_DestroyWindow(mSdlWindow);
    if(mSdlRenderer) SDL_DestroyRenderer(mSdlRenderer);
    setWidget(nullptr);
}

bool eMainWindow::initialize(const eWindowSettings& settings) {
    const auto& res = settings.fRes;
    const int w = res.width();
    const int h = res.height();
    const auto window = SDL_CreateWindow("eSlayer",
                                         w, h, 0);

    if(!window) {
        printf("Window could not be created! SDL Error: %s\n",
               SDL_GetError());
        return false;
    }
    const auto renderer = SDL_CreateRenderer(window, nullptr);
    if(!renderer) {
        printf("Renderer could not be created! SDL Error: %s\n",
               SDL_GetError());
        SDL_DestroyWindow(window);
        return false;
    }

    if(mSdlWindow) SDL_DestroyWindow(mSdlWindow);
    if(mSdlRenderer) SDL_DestroyRenderer(mSdlRenderer);
    mSdlWindow = window;
    mSdlRenderer = renderer;
    setResolution(res);
    setFullscreen(settings.fFullscreen);
    mSettings = settings;
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

    // const std::string icoPath = eGameDir::path("zeus.ico");
    // const auto icon = IMG_Load(icoPath.c_str());
    // SDL_SetWindowIcon(window, icon);
    return true;
}

void eMainWindow::setWidget(eWidget* const w) {
    if(mWidget) {
        mWidget->deleteLater();
    }
    mWidget = w;
}

eWidget* eMainWindow::takeWidget() {
    const auto w = mWidget;
    mWidget = nullptr;
    return w;
}

void eMainWindow::setResolution(const eResolution& res) {
    if(mSettings.fRes == res && !mFirstFullscrenSetting) return;
    mFirstResolutionSetting = false;
    mSettings.fRes = res;
    const int w = res.width();
    const int h = res.height();
    SDL_SetWindowSize(mSdlWindow, w, h);
}

void eMainWindow::setFullscreen(const bool f) {
    if(mSettings.fFullscreen == f && !mFirstFullscrenSetting) return;
    mFirstFullscrenSetting = false;
    mSettings.fFullscreen = f;
    SDL_SetWindowFullscreen(mSdlWindow, f ? SDL_WINDOW_FULLSCREEN : 0);
}

int eMainWindow::exec() {
    using namespace std::chrono;
    using namespace std::chrono_literals;

    eMouseButton button{eMouseButton::none};
    eMouseButton buttons{eMouseButton::none};

    SDL_Event e;

    const bool showFPS = true;
    const double fpsClamp = 2500;

    int c = 0;
    int fpsVal = 0;
    bool resetRenderTargets = false;
    while(!mQuit) {
        const auto fpsStart = high_resolution_clock::now();

        while(SDL_PollEvent(&e)) {
            float x, y;
            SDL_GetMouseState(&x, &y);
            const bool shift = mShiftPressed > 0;
            const bool ctrl = mCtrlPressed > 0;
            if(e.type == SDL_EVENT_QUIT) {
                mQuit = true;
            } else if(e.type == SDL_EVENT_WINDOW_MINIMIZED) {
                resetRenderTargets = true;
                while(SDL_WaitEvent(&e)) {
                    if(e.type == SDL_EVENT_WINDOW_RESTORED) {
                        break;
                    }
                }
            } else if(e.type == SDL_EVENT_WINDOW_EXPOSED) {
                resetRenderTargets = true;
            } else if(e.type == SDL_EVENT_RENDER_TARGETS_RESET ||
                      e.type == SDL_EVENT_RENDER_DEVICE_RESET) {
                resetRenderTargets = true;
            } else if(e.type == SDL_EVENT_MOUSE_MOTION) {
                const eMouseEvent me(x, y, shift, ctrl, buttons, button);
                if(mWidget) mWidget->mouseMove(me);
            } else if(e.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                switch(e.button.button) {
                case SDL_BUTTON_LEFT:
                    button = eMouseButton::left;
                    break;
                case SDL_BUTTON_RIGHT:
                    button = eMouseButton::right;
                    break;
                case SDL_BUTTON_MIDDLE:
                    button = eMouseButton::middle;
                    break;
                default: continue;
                }
                buttons = button | buttons;

                const eMouseEvent me(x, y, shift, ctrl, buttons, button);
                if(mWidget) mWidget->mousePress(me);
            } else if(e.type == SDL_EVENT_MOUSE_BUTTON_UP) {
                switch(e.button.button) {
                case SDL_BUTTON_LEFT:
                    button = eMouseButton::left;
                    break;
                case SDL_BUTTON_RIGHT:
                    button = eMouseButton::right;
                    break;
                case SDL_BUTTON_MIDDLE:
                    button = eMouseButton::middle;
                    break;
                default: continue;
                }
                buttons = buttons & ~button;
                const eMouseEvent me(x, y, shift, ctrl, buttons, button);
                if(mWidget) mWidget->mouseRelease(me);
            } else if(e.type == SDL_EVENT_MOUSE_WHEEL) {
                const eMouseWheelEvent me(x, y, shift, ctrl, buttons, e.wheel.y);
                if(mWidget) mWidget->mouseWheel(me);
            } else if(e.type == SDL_EVENT_KEY_DOWN) {
                const auto k = e.key.scancode;
                if(k == SDL_Scancode::SDL_SCANCODE_LSHIFT ||
                   k == SDL_Scancode::SDL_SCANCODE_RSHIFT) {
                    mShiftPressed++;
                } else if(k == SDL_Scancode::SDL_SCANCODE_LCTRL ||
                          k == SDL_Scancode::SDL_SCANCODE_RCTRL) {
                    mCtrlPressed++;
                }
                const eKeyPressEvent ke(x, y, shift, ctrl, buttons, k);
                if(mWidget) mWidget->keyPress(ke);
            } else if(e.type == SDL_EVENT_KEY_UP) {
                const auto k = e.key.scancode;
                if(k == SDL_Scancode::SDL_SCANCODE_LSHIFT ||
                   k == SDL_Scancode::SDL_SCANCODE_RSHIFT) {
                    mShiftPressed--;
                } else if(k == SDL_Scancode::SDL_SCANCODE_LCTRL ||
                          k == SDL_Scancode::SDL_SCANCODE_RCTRL) {
                    mCtrlPressed--;
                }
            }
        }

        if(resetRenderTargets) {
            resetRenderTargets = false;
            if(mWidget) mWidget->renderTargetsReset();
        }

        SDL_SetRenderDrawColor(mSdlRenderer, 0x0, 0x0, 0x0, 0xFF);
        SDL_RenderClear(mSdlRenderer);

        ePainter p(mSdlRenderer);

        eMusic::incTime();
        if(mWidget) {
            mWidget->paint(p);
        }

        if(showFPS) {
            p.setFont(eFonts::defaultFont(resolution()));
            p.drawText(0, 0, std::to_string(fpsVal), eFontColor::white);
        }

        SDL_RenderPresent(mSdlRenderer);

        for(const auto& s : mSlots) {
            s();
        }
        mSlots.clear();

        if(!mLoading.empty()) {
            const auto l = mLoading.back();
            mLoading.pop_back();
            l();
        }

        const auto fpsEnd = high_resolution_clock::now();
        const duration<double, std::milli> fpsElapsed = fpsEnd - fpsStart;
        const duration<double, std::milli> fpsDuration(1000./fpsClamp);
        const duration<double, std::milli> fpsSleep(fpsDuration - fpsElapsed);
        std::this_thread::sleep_for(fpsSleep);

        if(showFPS) {
            c++;
            if(c % 25 == 0) {
                const auto fpsEnd = high_resolution_clock::now();
                const duration<double, std::milli> fpsElapsed = fpsEnd - fpsStart;
                fpsVal = (int)std::round(1000./fpsElapsed.count());
            }
        }
    }

    return 0;
}

void eMainWindow::quit() {
    mQuit = true;
}

void eMainWindow::addSlot(const eSlot &slot) {
    mSlots.push_back(slot);
}

void eMainWindow::addLoad(const eAction& l) {
    mLoading.push_back(l);
}

