#ifndef ESPACEEFFECT_H
#define ESPACEEFFECT_H

#include "eeffect.h"

class eSpaceEffect : public eEffect {
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
    float mSize = 0.002f;

    float mTime = 0.f;

    std::vector<float> mSpeed0;

    std::vector<float> mY;
    std::vector<float> mX;

    std::vector<SDL_Vertex> mVerts;
    std::vector<int> mIndices;
};

#endif // ESPACEEFFECT_H
