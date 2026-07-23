#include "eskillbutton.h"

#include "../../textures/euitextures.h"
#include "ehoverwidget.h"

#include <eSlayerHelpers/eskills.h>
#include <eSlayerHelpers/evectorhelpers.h>

std::map<int, int> eSkillButton::sLeftMap;
std::map<int, int> eSkillButton::sRightMap;

std::vector<eSkillButton*> eSkillButton::sInstances;

eSkillButton::~eSkillButton() {
    eVectorHelpers::remove(sInstances, this);
}

void eSkillButton::initialize(const int schoice) {
    mSchoice = schoice;
    if(mSchoice == static_cast<int>(eSkillChoice::left) ||
       mSchoice == static_cast<int>(eSkillChoice::right)) {
        sInstances.emplace_back(this);
    }
    const int attackSkillId = eSkills::sSkills.id("attack");
    setSkillId(attackSkillId);
    setNoPadding();
    fitContent();
    updateText();
}

void eSkillButton::setSkillId(const int skillId) {
    if(skillId == -1) return;
    mSkillId = skillId;
    const auto& skill = eSkills::sSkills.get(skillId);
    const int iconId = skill.fIconId;
    if(iconId == -1) return;
    const auto& icon = eUITextures::sSkillIcons.get(iconId);
    setTexture(icon);
    updateText();
}

void eSkillButton::setLevelReq(const int levelReq) {
    mLevelReq = levelReq;
}

void eSkillButton::setLevelReqMet(const bool met) {
    mLevelReqMet = met;
}

void eSkillButton::setTopRightText(const std::string& text) {
    setText(mTopRight, text, eAlignment::top | eAlignment::right);
}

void eSkillButton::setBottomRightText(const std::string& text) {
    setText(mBottomRight, text, eAlignment::bottom | eAlignment::right);
}

void eSkillButton::setCooldown(const float time) {
    mCooldown = time;
}

void eSkillButton::setCooldownMax(const float maxTime) {
    mMaxCooldown = maxTime;
}

void eSkillButton::setCanUse(const bool c) {
    mCanUse = c;
}

void eSkillButton::updateText() {
    if(mSchoice == static_cast<int>(eSkillChoice::left)) {
        updateText(sLeftMap);
    } else if(mSchoice == static_cast<int>(eSkillChoice::right)) {
        updateText(sRightMap);
    }
}

SDL_FPoint eSkillButton::center() const {
    SDL_FPoint result;
    result.x = x() + 0.5f*width();
    result.y = y() + 0.5f*height();
    return result;
}

bool eSkillButton::mouseMoveEvent(const eMouseEvent& e) {
    (void)e;
    return true;
}

bool eSkillButton::mouseEnterEvent(const eMouseEvent& e) {
    (void)e;
    const auto rect = globalRect();
    eHoverWidget::sSetHoverSkill(
        mSkillId, mSchoice == -1, rect,
        mLevelReq, mLevelReqMet);
    return true;
}

bool eSkillButton::mouseLeaveEvent(const eMouseEvent& e) {
    (void)e;
    eHoverWidget::sSetHoverSkill(
        -1, mSchoice == -1);
    return true;
}

bool eSkillButton::keyPressEvent(const eKeyPressEvent& e) {
    const auto key = e.key();
    if(key == SDL_SCANCODE_F1) {
        setHotkey(1);
    } else if(key == SDL_SCANCODE_F2) {
        setHotkey(2);
    } else if(key == SDL_SCANCODE_F3) {
        setHotkey(3);
    } else if(key == SDL_SCANCODE_F4) {
        setHotkey(4);
    } else if(key == SDL_SCANCODE_F5) {
        setHotkey(5);
    } else if(key == SDL_SCANCODE_F6) {
        setHotkey(6);
    } else if(key == SDL_SCANCODE_F7) {
        setHotkey(7);
    } else if(key == SDL_SCANCODE_F8) {
        setHotkey(8);
    } else {
        return false;
    }
    return true;
}

void eSkillButton::paintEvent(ePainter& p) {
    const auto& tex = texture();
    if(!tex) return;
    const auto rect = eWidget::rect();
    const auto align = textAlignment();
    const float gb = mCanUse ? 1.f : 0.5f;
    {
        const eTextureColorHolder holder(true, 1.f, gb, gb, 1.f, tex);
        p.drawTexture(rect, tex, align);
    }
    if(mCooldown <= 0.f || mMaxCooldown <= 0.f) {
        return;
    }
    const float frac = mCooldown/mMaxCooldown;
    const int h = height();
    const int w = width();
    const int hfrac = frac*h;
    const SDL_Rect clipRect{0, h - hfrac, w, hfrac};
    p.setClipRect(&clipRect);
    {
        const eTextureColorHolder holder(true, 0.5f, gb*0.5f, gb*0.5f, 1.f, tex);
        p.drawTexture(rect, tex, align);
    }
    p.setClipRect(nullptr);
}

void eSkillButton::setText(eLabel*& ptr,
                           const std::string& text,
                           const eAlignment align) {
    if(!ptr) {
        ptr = new eLabel(window());
        ptr->setNoPadding();
        const auto& res = resolution();
        const int fontSize = res.smallFontSize();
        const auto font = eFonts::textFont(fontSize);
        ptr->setFont(font);
        addWidget(ptr);
    }
    ptr->setText(text);
    ptr->fitContent();
    ptr->align(align);
}

void eSkillButton::setHotkey(const int fkey) {
    sRemoveHotkey(fkey);
    if(mSchoice == static_cast<int>(eSkillChoice::left)) {
        sAddHotkey(sLeftMap, mSkillId, fkey);
    } else if(mSchoice == static_cast<int>(eSkillChoice::right)) {
        sAddHotkey(sRightMap, mSkillId, fkey);
    }
    for(const auto b : sInstances) {
        b->updateText();
    }
}

void eSkillButton::updateText(const std::map<int, int>& map) {
    for(const auto& it : map) {
        if(it.second != mSkillId) continue;
        setTopRightText("F" + std::to_string(it.first));
        return;
    }
    setTopRightText("");
}

void eSkillButton::sRemoveHotkey(const int fkey) {
    sRemoveHotkey(sLeftMap, fkey);
    sRemoveHotkey(sRightMap, fkey);
}

void eSkillButton::sRemoveHotkey(std::map<int, int>& map,
                                 const int fkey) {
    map.erase(fkey);
}

void eSkillButton::sAddHotkey(std::map<int, int>& map,
                              const int skillId,
                              const int fkey) {
    map[fkey] = skillId;
}
