#pragma once

#include <llapi/utils/FileHelper.h>
#include <Nlohmann/json.hpp>

namespace Settings {
    extern bool ModifyDragonHealth;
    extern int DragonHealth;
    extern bool AttackCrystalOnly;
    extern bool ModifyCrystalExplosion;
    extern bool CrystalExplosionFire;
    extern int CrystalExplosionPower;
    extern bool ModifyDragonDamage; 
    extern int DragonBreathDamage;
    extern int DirectAttackDamage;
    extern bool PlayerDamageLimit;
    extern int MaxDamagePerTime;
    extern bool ImmuneDamageWhileLanding;
    extern bool IfCrystalHeal;
    extern int HealCostTime;
    extern bool DragonExplosion;//
    extern int DragonExplosionPower;//
    extern bool DragonLightning;//
    extern int LightningDamage;//
    extern bool ReflectDamage;
    extern float ReflectPercentage;
    extern bool SpawnChildMob;//
    extern std::vector<std::string> ChildMobList;//
    extern bool NatureRegeneration;
    extern int IntervalTicks;
    extern int RegenerationEachTime;
    extern bool EnableReward;
    extern bool UseLLMoney;
    extern std::string MoneyScore;
    extern int TotalReward;
    extern bool ForceKonckback;
    extern bool CrystalExplodeEffect;
    extern std::vector<int> CrystalEffectId;
    extern int CrystalEffectDurationTicks;
    extern int CrystalEffectAmplifier;
    extern bool DragonExplodeEffect;
    extern std::vector<int> DragonEffectId;
    extern int DragonEffectDurationTicks;
    extern int DragonEffectAmplifier;
    extern bool DragonHealOnKillMob;
    extern int HealOnKillMob;
    extern bool DragonDestroyItem;
    extern bool CrystalDestroyItem;
    extern bool DragonArmor;
    extern unsigned short ArmorValue;
    extern unsigned short ToughnessValue;

    extern nlohmann::ordered_json globaljson();
    void initjson(nlohmann::ordered_json json);
    void WriteDefaultConfig(const std::string& fileName);
    void LoadConfigFromJson(const std::string& fileName);
    void reloadJson(const std::string& fileName);
}

extern void loadConfig();
extern void IniData();