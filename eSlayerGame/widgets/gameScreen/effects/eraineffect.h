#ifndef ERAINEFFECT_H
#define ERAINEFFECT_H

#include "eeffect.h"

class eRainEffect : public eEffect {
protected:
    void apply(SDL_Renderer* const r,
               std::shared_ptr<eTexture>& to,
               std::shared_ptr<eTexture>& tmp) override;
    void initialize(const eEffectSettings& settings,
                    const int w, const int h,
                    const float* centerX,
                    const float* centerY,
                    const bool fadeIn) override;
    void stop() override;
private:
    void increment(const float by);

    bool mFade = false;

    float mSpeed = 0.025f;
    uint16_t mCount = 0;
    float mLength = 0.015f;
    float mThick = 0.001f;
    float mTilt = 0.1f;
    float mFadeSpeed = 5.f;

    float mTime = 0.f;
    float mRemTime = 0.f;

    std::vector<float> mX0;

    std::vector<float> mX;
    std::vector<float> mY;

    std::vector<SDL_Vertex> mVerts;
    std::vector<int> mIndices;
};

#endif // ERAINEFFECT_H
