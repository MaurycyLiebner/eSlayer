#ifndef EDISTORTEFFECT_H
#define EDISTORTEFFECT_H

#include "eeffect.h"

class eDistortEffect : public eEffect {
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
    std::vector<float> generateRandomMap() const;
    void generateRandomMap(std::vector<float>& result) const;
    void increment(const float by);

    bool mFadeOut = false;
    bool mFadingOut = false;

    float mSpeed = 0.025f;
    float mScale = 0.025f;

    uint16_t mXDiv;
    uint16_t mYDiv;
    uint16_t mNVerts;

    std::vector<float> mXTexCoords;
    std::vector<float> mYTexCoords;

    std::vector<float> mXR1;
    std::vector<float> mYR1;

    std::vector<float> mXR2;
    std::vector<float> mYR2;

    float mTime = 0.f;

    std::vector<SDL_Vertex> mVerts;
    std::vector<int> mIndices;
};

#endif // EDISTORTEFFECT_H
