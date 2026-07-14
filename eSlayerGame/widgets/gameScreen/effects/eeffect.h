#ifndef EEFFECT_H
#define EEFFECT_H

#include "../../../textures/etexture.h"

#include <vector>

class eEffect {
    friend class eEffects;
protected:
    virtual void apply(SDL_Renderer* const r,
                       std::shared_ptr<eTexture>& to,
                       std::shared_ptr<eTexture>& tmp) = 0;
    virtual void initialize(const int w, const int h) {};
};

class eEffects {
public:
    void initialize(SDL_Renderer* const r,
                    const int w, const int h);
    void apply(SDL_Renderer* const r,
               std::shared_ptr<eTexture>& to);

    void addEffect(const std::shared_ptr<eEffect>& e);
    void clearEffects();
private:
    int mWidth = 0;
    int mHeight = 0;

    std::shared_ptr<eTexture> mTmp;
    std::vector<std::shared_ptr<eEffect>> mEffects;
};

#endif // EEFFECT_H
