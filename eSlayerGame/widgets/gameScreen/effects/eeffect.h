#ifndef EEFFECT_H
#define EEFFECT_H

#include "../../../textures/etexture.h"

#include <eSlayerHelpers/eeffectsettings.h>

#include <vector>

class eEffect {
    friend class eEffects;
protected:
    virtual void apply(SDL_Renderer* const r,
                       std::shared_ptr<eTexture>& to,
                       std::shared_ptr<eTexture>& tmp) = 0;
    virtual void initialize(const eEffectSettings& settings,
                            const int w, const int h,
                            const float* centerX,
                            const float* centerY,
                            const bool fadeIn);
    virtual void stop();

    bool done() const { return mDone; }

    bool mDone = false;

    eEffectSettings mSettings;

    int mWidth = 0;
    int mHeight = 0;

    const float* mCenterX = nullptr;
    const float* mCenterY = nullptr;
};

class eEffects {
public:
    void initialize(SDL_Renderer* const r,
                    const int w, const int h,
                    const float* centerX,
                    const float* centerY);
    void apply(SDL_Renderer* const r,
               std::shared_ptr<eTexture>& to);

    void addEffect(const eEffectSettings& settings,
                   const bool fadeIn);
    void clearEffects();
private:
    int mWidth = 0;
    int mHeight = 0;

    const float* mCenterX = nullptr;
    const float* mCenterY = nullptr;

    std::shared_ptr<eTexture> mTmp;
    std::vector<std::shared_ptr<eEffect>> mEffects;
};

#endif // EEFFECT_H
