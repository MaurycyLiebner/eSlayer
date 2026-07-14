#ifndef EDISTORTEFFECT_H
#define EDISTORTEFFECT_H

#include "eeffect.h"

class eDistortEffect : public eEffect {
protected:
    void apply(SDL_Renderer* const r,
               std::shared_ptr<eTexture>& to,
               std::shared_ptr<eTexture>& tmp) override;
    void initialize(const int w, const int h) override;
private:
    uint16_t mXDiv;
    uint16_t mYDiv;
    std::vector<SDL_Vertex> mVerts;
    std::vector<int> mIndices;
};

#endif // EDISTORTEFFECT_H
