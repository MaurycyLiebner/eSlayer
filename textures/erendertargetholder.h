#ifndef ERENDERTARGETHOLDER_H
#define ERENDERTARGETHOLDER_H

#include <SDL3/SDL.h>

class eRenderTargetHolder {
public:
    eRenderTargetHolder(SDL_Renderer* const r,
                        SDL_Texture * const target);
    ~eRenderTargetHolder();

    eRenderTargetHolder(const eRenderTargetHolder&) = delete;
    eRenderTargetHolder& operator=(const eRenderTargetHolder&) = delete;
private:
    SDL_Renderer* const mR;
    SDL_Texture* mTmp = nullptr;
};

#endif // ERENDERTARGETHOLDER_H
