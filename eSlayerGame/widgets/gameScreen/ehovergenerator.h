#ifndef EHOVERGENERATOR_H
#define EHOVERGENERATOR_H

#include "../efonts.h"
#include "../efontcolor.h"
#include "../../textures/etexture.h"
#include "../ealignment.h"

#include <eSlayerHelpers/emodifier.h>

#include <string>
#include <memory>
#include <vector>

class ePainter;
class eResolution;

class eHoverGenerator {
public:
    eHoverGenerator(const eResolution& res);

    void addSpace(SDL_Renderer* const r);
    void addText(SDL_Renderer* const r,
                 const std::string& text,
                 const eFontColor color);
    void addValue(SDL_Renderer* const r,
                  const int g, const int s,
                  const int min, const int max,
                  const int skillId,
                  const int classId,
                  const eFontColor color,
                  const eModifierType type = eModifierType::none);
    void addValue(SDL_Renderer* const r,
                  const int g, const int s,
                  const int min, const int max,
                  const eFontColor color,
                  const eModifierType type = eModifierType::none);
    void addValue(SDL_Renderer* const r,
                  const int g, const int s,
                  const int value,
                  const eFontColor color,
                  const eModifierType type = eModifierType::none);

    std::shared_ptr<eTexture>
    generate(const eResolution& res,
             SDL_Renderer* const r) const;

    static void sPaint(const int w, const int h,
                       int x, int y,
                       const eResolution& res,
                       const std::shared_ptr<eTexture>& tex,
                       ePainter& p, const eAlignment align);
private:
    eFont font;
    int totalHeight = 0;
    int maxWidth = 0;
    std::vector<std::shared_ptr<eTexture>> lines;
};

#endif // EHOVERGENERATOR_H
