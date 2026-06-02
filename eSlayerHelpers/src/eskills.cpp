#include "eSlayerHelpers/eskills.h"

#include "eSlayerHelpers/eboostcursetypes.h"
#include "eSlayerHelpers/eauratypes.h"
#include "eSlayerHelpers/efileloaderbase.h"
#include "eSlayerHelpers/epacket.h"
#include "eSlayerHelpers/estats.h"

bool eSkills::sLoaded = false;
eStringIdMapVector<eSkill> eSkills::sSkills;
const int eSkills::sMaxSkillLevel = 99;

const float eSkill::sRadiusMax = 12.75f;
const float eSkill::sSpeedMax = 1.f;
const float eSkill::sRangeMax = 25.5f;
const float eSkill::sTimeMax = 2550.f;

void eSkills::load() {
    if(sLoaded) return;
    sLoaded = true;

    const auto dir = "Skills";

    std::vector<std::string> skills;
    try {
        const auto jdata = eFileLoaderBase::parse(dir, "skills.json");
        skills = jdata.get<std::vector<std::string>>();
    } catch(...) {
        eRuntimeThrow("Failed to parse " + dir + "/skills.json");
    }

    sSkills.reserve(skills.size());

    for(const auto& name : skills) {
        try {
            const int oldId = sSkills.id(name);
            if(oldId != -1) {
                eExceptions::showDialog("Duplicate skill '" + name + "' in " + dir + "/skills.json");
                continue;
            }
            std::map<eModifierType, eModifier> modifiers;

            eSkill skill;
            const auto jdata = eFileLoaderBase::parse(dir, name + ".json");
            const std::string typeStr = jdata["type"];
            skill.fIcon = jdata["icon"];

            skill.fPath = jdata.value("path", "linear");
            skill.fMissileStr = jdata.value("missile", "none");
            skill.fUnitStr = jdata.value("character", "none");
            skill.fMissileEnemyFindRange = jdata.value("enemyFindRange", 0.f);
            const float speed = jdata.value("speed", 50u);
            skill.fSpeed = ePacket::toFloatU8(speed, eSkill::sSpeedMax);
            skill.fMaxAngle = jdata.value("maxAngle", 0.f);
            skill.fAngleAdjust = jdata.value("angleAdjust", true);

            if(typeStr == "attack") {
                skill.fType = eSkillType::attack;
                skill.fCastRange = 0.f;
            } else if(typeStr == "smite") {
                skill.fType = eSkillType::smite;
                skill.fCastRange = 0.f;
            } else if(typeStr == "kick") {
                skill.fType = eSkillType::kick;
                skill.fCastRange = 0.f;
            } else if(typeStr == "missile") {
                skill.fType = eSkillType::missile;
                const uint8_t range = jdata.value("range", 75u);
                skill.fRange = ePacket::toFloatU8(range, eSkill::sRangeMax);
                skill.fCastRange = 8.f;
            } else if(typeStr == "nova") {
                skill.fType = eSkillType::nova;
                const uint8_t range = jdata.value("range", 35u);
                skill.fRange = ePacket::toFloatU8(range, eSkill::sRangeMax);
                skill.fCastRange = 4.f;
            } else if(typeStr == "shoot") {
                skill.fType = eSkillType::shoot;
            } else if(typeStr == "throw") {
                skill.fType = eSkillType::throw_;
            } else if(typeStr == "wall") {
                skill.fType = eSkillType::wall;
                const uint16_t time = jdata.value("time", 25u);
                skill.fTime = ePacket::toFloatU8(time, eSkill::sTimeMax);
                skill.fPath = "static";
                skill.fCastRange = 8.f;
            } else if(typeStr == "summon") {
                skill.fType = eSkillType::summon;
                skill.fCastRange = 8.f;
            } else if(typeStr == "passive") {
                skill.fType = eSkillType::passive;
            } else if(typeStr == "boostCurse") {
                skill.fType = eSkillType::boostCurse;

                const uint8_t time = jdata.value("time", 25u);
                skill.fTime = ePacket::toFloatU8(time, eSkill::sTimeMax);

                const auto boostCurseTypeStr = jdata.value("boostCurseType", "");
                const int id = eBoostCurseTypes::sTypes.id(boostCurseTypeStr);
                if(id <= 0) {
                    eRuntimeThrow("Invalid \"boostCurseType\" \"" + boostCurseTypeStr +
                                  "\" in \"" + dir + "/" + name + ".json\"");
                }
                skill.fBoostCurseType = static_cast<eBoostCurseType>(id);

                const auto boostCurseTargetStr = jdata.value("boostCurseTarget", "");
                if(boostCurseTargetStr == "enemyArea") {
                    skill.fBoostCurseTarget = eBoostCurseTarget::enemyArea;
                } else {
                    eRuntimeThrow("Invalid \"boostCurseTarget\" \"" + boostCurseTargetStr +
                                  "\" in \"" + dir + "/" + name + ".json\"");
                }

                skill.fAreaMissileStr = jdata.value("areaMissile", "none");
            } else if(typeStr == "aura") {
                skill.fType = eSkillType::aura;

                const auto auraTypeStr = jdata.value("auraType", "");
                const int id = eAuraTypes::sTypes.id(auraTypeStr);
                if(id < 0) {
                    eRuntimeThrow("Invalid \"auraType\" \"" + auraTypeStr +
                                  "\" in \"" + dir + "/" + name + ".json\"");
                }
                skill.fAuraType = static_cast<eAuraType>(id);

                const auto auraTargetStr = jdata.value("auraTarget", "");
                if(auraTargetStr == "allies") {
                    skill.fAuraTarget = eAuraTarget::allies;
                } else if(auraTargetStr == "enemies") {
                    skill.fAuraTarget = eAuraTarget::enemies;
                } else {
                    eRuntimeThrow("Invalid \"auraTarget\" \"" + auraTargetStr +
                                  "\" in \"" + dir + "/" + name + ".json\"");
                }

                skill.fAreaMissileStr = jdata.value("areaMissile", "none");
            } else {
                eRuntimeThrow("Unrecognized skill type \"" + typeStr + "\" for " + name);
            }

            int count = jdata.value("count", 0);
            float cooldown = jdata.value("cooldown", 0.f);
            float manaCost = jdata.value("manaCost", 0.f);
            uint8_t radius = jdata.value("radius", 0);

            skill.fCastAnims = jdata.value("castAnimations", std::vector<std::string>());
            if(jdata.contains("levels")) {
                const auto& levels = jdata["levels"];
                eModsCollectionLevel::parseLevels(
                    levels, skill.fLevels,
                    eSkills::sMaxSkillLevel,
                    count, cooldown, manaCost,
                    radius);
            }
            if(jdata.contains("synergies")) {
                eModsCollection totalMods;
                const auto& synergies = jdata["synergies"];
                skill.fSynergies.reserve(synergies.size());
                for(auto& [name, levels] : synergies.items()) {
                    auto& synergy = skill.fSynergies.emplace_back();
                    synergy.fSkillStr = name;
                    eModsCollectionLevel::parseLevels(
                        levels, synergy.fBoostLevels,
                        eSkills::sMaxSkillLevel);
                }
            }
            sSkills.add(name, skill);
        } catch(...) {
            eRuntimeThrow("Failed to parse skill \"" + name + "\".");
        }
    }

    for(const auto& it : sSkills) {
        auto& skill = it.fValue;
        auto& synergies = skill.fSynergies;
        for(auto& s : synergies) {
            const auto& name = s.fSkillStr;
            const int id = sSkills.id(name);
            s.fSkillId = id;
            if(id == -1) {
                eExceptions::showDialog("Unrecognized synergy \"" + name + "\".");
            }
        }
    }
}
