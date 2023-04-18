#include <llapi/utils/FileHelper.h>
#include <Nlohmann/json.hpp>
#include <LoggerAPI.h>

extern Logger logger;
extern bool isDragonAlive;

#define TRJ(key1, key2, val)                                     \
    if (json.find(key1) != json.end())                           \
    {                                                            \
        if (json.at(key1).find(key2) != json.at(key1).end()) {   \
            const nlohmann::json &out2 = json.at(key1).at(key2); \
            out2.get_to(val);                                    \
        }                                                        \
    }

namespace Settings
{
    bool ModifyDragonHealth = true;
    int DragonHealth = 2000;
    bool AttackCrystalOnly = true;
    bool ModifyCrystalExplosion = true;
    bool CrystalExplosionFire = true;
    int CrystalExplosionPower = 20;
    bool ModifyDragonDamage = true; 
    int DragonBreathDamage = 15;
    int DirectAttackDamage = 45;
    bool PlayerDamageLimit = true;
    int MaxDamagePerTime = 20;
    bool ImmuneDamageWhileLanding = true;
    bool IfCrystalHeal = true;
    int HealCostTime = 120;
    bool DragonExplosion = true;
    int DragonExplosionPower = 6;
    bool DragonLightning = true;
    int LightningDamage = 20;
    bool ReflectDamage = true;
    float ReflectPercentage = 20;
    bool SpawnChildMob = true;
    bool DragonDestroyItem = false;
    bool CrystalDestroyItem = false;
    std::vector<std::string> ChildMobList = {
        "minecraft:wither_skeleton"
    };
    bool NatureRegeneration = false;
    int IntervalTicks = 20;
    int RegenerationEachTime = 1;
    bool EnableReward = true;
    bool UseLLMoney = true;
    std::string MoneyScore = "money";
    int TotalReward = 114514;
    bool ForceKonckback = true;
    bool CrystalExplodeEffect = true;
    std::vector<int> CrystalEffectId = {
        2,
        9,
        15,
        17,
        20
    };
    int CrystalEffectDurationTicks = 1200;
    int CrystalEffectAmplifier = 4;
    bool DragonExplodeEffect = true;
    std::vector<int> DragonEffectId = {
        9,
        17,
        20
    };
    int DragonEffectDurationTicks = 600;
    int DragonEffectAmplifier = 4;
    bool DragonHealOnKillMob = true;
    int HealOnKillMob = 4;
    bool DragonArmor = true;
    unsigned short ArmorValue = 4;
    unsigned short ToughnessValue = 0;

    nlohmann::ordered_json globaljson()
    {
        nlohmann::ordered_json json;
        json["ModifyDragonHealth"]["Enabled"] = ModifyDragonHealth;
        json["ModifyDragonHealth"]["DragonHealth"] = DragonHealth;
        json["AttacktoDestroyCrystalOnly"]["Enabled"] = AttackCrystalOnly;
        json["ModifyCrystalExplosion"]["Enabled"] = ModifyCrystalExplosion;
        json["ModifyCrystalExplosion"]["isFire"] = CrystalExplosionFire;
        json["ModifyCrystalExplosion"]["Power"] = CrystalExplosionPower;
        json["ModifyCrystalExplosion"]["DestoryItemEntity"] = CrystalDestroyItem;
        json["ModifyDragonDamage"]["Enabled"] = ModifyDragonDamage;
        json["ModifyDragonDamage"]["DragonBreathDamage"] = DragonBreathDamage;
        json["ModifyDragonDamage"]["DirectAttackDamage"] = DirectAttackDamage;
        json["PlayerDamageLimit"]["Enabled"] = PlayerDamageLimit;
        json["PlayerDamageLimit"]["MaxDamagePerTime"] = MaxDamagePerTime;
        json["ImmuneDamageWhileLanding"]["Enabled"] = ImmuneDamageWhileLanding;
        json["CrystalRegeneration"]["Enabled"] = IfCrystalHeal;
        json["CrystalRegeneration"]["Time"] = HealCostTime;
        json["DragonExplosion"]["Enabled"] = DragonExplosion;
        json["DragonExplosion"]["Power"] = DragonExplosionPower;
        json["DragonExplosion"]["DestoryItemEntity"] = DragonDestroyItem;
        json["DragonLightning"]["Enabled"] = DragonLightning;
        json["DragonLightning"]["LightningDamage"] = LightningDamage;
        json["ReflectDamage"]["Enabled"] = ReflectDamage;
        json["ReflectDamage"]["Percentage"] = ReflectPercentage;
        json["SpawnChildMobs"]["Enabled"] = SpawnChildMob;
        json["SpawnChildMobs"]["ChildMobList"] = ChildMobList;
        json["DragonNatureRegeneration"]["Enabled"] = NatureRegeneration;
        json["DragonNatureRegeneration"]["IntervalTicks"] = IntervalTicks;
        json["DragonNatureRegeneration"]["RegenerationEachTime"] = RegenerationEachTime;
        json["DragonKillReward"]["Enabled"] = EnableReward;
        json["DragonKillReward"]["UseLLMoney"] = UseLLMoney;
        json["DragonKillReward"]["ScoreBoard"] = MoneyScore;
        json["DragonKillReward"]["TatalReward"] = TotalReward;
        json["DragonForceKnockback"]["Enabled"] = ForceKonckback;
        json["CrystalExplodeEffect"]["Enabled"] = CrystalExplodeEffect;
        json["CrystalExplodeEffect"]["CrystalExplodeEffectId"] = CrystalEffectId;
        json["CrystalExplodeEffect"]["CrystalEffectDurationTicks"] = CrystalEffectDurationTicks;
        json["CrystalExplodeEffect"]["CrystalEffectAmplifier"] = CrystalEffectAmplifier;
        json["DragonUsedEffect"]["Enabled"] = DragonExplodeEffect;
        json["DragonUsedEffect"]["DragonEffectId"] = DragonEffectId;
        json["DragonUsedEffect"]["CrystalEffectDurationTicks"] = DragonEffectDurationTicks;
        json["DragonUsedEffect"]["CrystalEffectAmplifier"] = DragonEffectAmplifier;
        json["DragonHealOnKillMob"]["Enabled"] = DragonHealOnKillMob;
        json["DragonHealOnKillMob"]["Value"] = HealOnKillMob;
        json["DragonArmor"]["Enabled"] = DragonArmor;
        json["DragonArmor"]["ArmorValue"] = ArmorValue;
        json["DragonArmor"]["ToughnessValue"] = ToughnessValue;
        return json;
    }

    void initjson(nlohmann::json json)
    {
        TRJ("DragonAttribute", "Enabled", ModifyDragonHealth);
        TRJ("DragonAttribute", "DragonHealth", DragonHealth);
        TRJ("AttacktoDestroyCrystalOnly", "Enabled", AttackCrystalOnly);
        TRJ("ModifyCrystalExplosion", "Enabled", ModifyCrystalExplosion);
        TRJ("ModifyCrystalExplosion", "isFire", CrystalExplosionFire);
        TRJ("ModifyCrystalExplosion", "Power", CrystalExplosionPower);
        TRJ("ModifyCrystalExplosion", "DestoryItemEntity", CrystalDestroyItem);
        TRJ("ModifyDragonDamage", "Enabled", ModifyDragonDamage);
        TRJ("ModifyDragonDamage", "DragonBreathDamage", DragonBreathDamage);
        TRJ("ModifyDragonDamage", "DirectAttackDamage", DirectAttackDamage);
        TRJ("PlayerDamageLimit", "Enabled", PlayerDamageLimit);
        TRJ("PlayerDamageLimit", "MaxDamagePerTime", MaxDamagePerTime);
        TRJ("ImmuneDamageWhileLanding", "Enabled", ImmuneDamageWhileLanding);
        TRJ("CrystalRegeneration", "Enabled", IfCrystalHeal);
        TRJ("CrystalRegeneration", "Time", HealCostTime);
        TRJ("DragonExplosion", "Enabled", DragonExplosion);
        TRJ("DragonExplosion", "Power", DragonExplosionPower);
        TRJ("DragonExplosion", "DestoryItemEntity", DragonDestroyItem);
        TRJ("DragonLightning", "Enabled", DragonLightning);
        TRJ("DragonLightning", "LightningDamage", LightningDamage);
        TRJ("ReflectDamage", "Enabled", ReflectDamage);
        TRJ("ReflectDamage", "Percentage", ReflectPercentage);
        TRJ("SpawnChildMobs", "Enabled", SpawnChildMob);
        TRJ("SpawnChildMobs", "ChildMobList", ChildMobList);
        TRJ("DragonNatureRegeneration", "Enabled", NatureRegeneration);
        TRJ("DragonNatureRegeneration", "IntervalTicks", IntervalTicks);
        TRJ("DragonNatureRegeneration", "RegenerationEachTime", RegenerationEachTime);
        TRJ("DragonKillReward", "Enabled", EnableReward);
        TRJ("DragonKillReward", "UseLLMoney", UseLLMoney);
        TRJ("DragonKillReward", "ScoreBoard", MoneyScore);
        TRJ("DragonKillReward", "TatalReward", TotalReward);
        TRJ("DragonForceKnockback", "Enabled", ForceKonckback);
        TRJ("CrystalExplodeEffect", "Enabled", CrystalExplodeEffect);
        TRJ("CrystalExplodeEffect", "CrystalExplodeEffectId", CrystalEffectId);
        TRJ("CrystalExplodeEffect", "CrystalEffectDurationTicks", CrystalEffectDurationTicks);
        TRJ("CrystalExplodeEffect", "CrystalEffectAmplifier", CrystalEffectAmplifier);
        TRJ("DragonUsedEffect", "Enabled", DragonExplodeEffect);
        TRJ("DragonUsedEffect", "DragonEffectId", DragonEffectId);
        TRJ("DragonUsedEffect", "CrystalEffectDurationTicks", DragonEffectDurationTicks);
        TRJ("DragonUsedEffect", "CrystalEffectAmplifier", DragonEffectAmplifier);
        TRJ("DragonHealOnKillMob", "Enabled", DragonHealOnKillMob);
        TRJ("DragonHealOnKillMob", "Value", HealOnKillMob);
        TRJ("DragonArmor", "Enabled", DragonArmor);
        TRJ("DragonArmor", "ArmorValue", ArmorValue);
        TRJ("DragonArmor", "ToughnessValue", ToughnessValue);
    }

    void WriteDefaultConfig(const std::string &fileName)
    {
        std::ofstream file(fileName);
        if (!file.is_open())
        {
            logger.error("Fail to open config file!");
            return;
        }
        auto json = globaljson();
        file << json.dump(4);
        file.close();
    }

    void LoadConfigFromJson(const std::string &fileName)
    {
        std::ifstream file(fileName);
        if (!file.is_open())
        {
            logger.error("Fail to open config file!");
            return;
        }
        nlohmann::ordered_json json;
        file >> json;
        file.close();
        initjson(json);
        WriteDefaultConfig(fileName);
    }
    void reloadJson(const std::string &fileName)
    {
        std::ofstream file(fileName);
        if (file)
        {
            file << globaljson().dump(4);
        }
        else
        {
            logger.error("Error in config file!");
        }
        file.close();
    }
}

void IniData() {
    if(!std::filesystem::exists("plugins/FerociousEnderDragon/data/data.json")){
        std::ofstream DefaultFile("plugins/FerociousEnderDragon/data/data.json");
        if (!DefaultFile.is_open()) {
            return;
        }
        nlohmann::json json;
        json.clear();
        json["isDragonAlive"] = false;
        DefaultFile << json.dump(4);
        DefaultFile.close();
    }
}

#include "Fuctions.h"
void loadConfig() { 
    if (!std::filesystem::exists("plugins/FerociousEnderDragon")){
        std::filesystem::create_directories("plugins/FerociousEnderDragon");	
    }
    if (!std::filesystem::exists("plugins/FerociousEnderDragon/DeathMessages.json")) {
        std::ofstream DefaultFile("plugins/FerociousEnderDragon/DeathMessages.json");
        if (!DefaultFile.is_open()) {
            return;
        }
        nlohmann::json json;
        json["death.ferociousEnderDragon.explosion"] = "%1$s只因再多看了一眼末影龙就爆炸了";
        json["death.ferociousEnderDragon.lightning"] = "%1$s败落于末影龙制造的闪电之下";
        json["death.ferociousEnderDragon.dragonBreath"] = "%1$s被末影龙的龙息烤熟了";
        json["death.ferociousEnderDragon.collision"] = "%1$s最终还是没能躲过末影龙的冲撞";
        json["death.ferociousEnderDragon.sonicBoom"] = "%1$s只因再靠近了一点末影龙就融化了";
        DefaultFile << json.dump(4);
        DefaultFile.close();
    }
    if (std::filesystem::exists("plugins/FerociousEnderDragon/config.json")) {
        Settings::LoadConfigFromJson("plugins/FerociousEnderDragon/config.json");
    }
    else {
        logger.warn("Config file does not exist, created automatically.");
        Settings::WriteDefaultConfig("plugins/FerociousEnderDragon/config.json");
    }
    IniData();
    if (!std::filesystem::exists("plugins/FerociousEnderDragon/data")){
        std::filesystem::create_directories("plugins/FerociousEnderDragon/data");	
    }
    if (!std::filesystem::exists("plugins/FerociousEnderDragon/data.json")){
        std::filesystem::remove("plugins/FerociousEnderDragon/data.json");	
    }
    if (!std::filesystem::exists("plugins/FerociousEnderDragon/data/player.json")){
        ResetPlayerData();
    }
}