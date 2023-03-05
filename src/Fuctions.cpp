#include "setting.h"
#include <mc/Vec3.hpp>
#include <mc/Spawner.hpp>
#include <mc/ActorDefinitionIdentifier.hpp>
#include <mc/Level.hpp>
#include <ScheduleAPI.h>
#include <mc/Actor.hpp>
#include <mc/CompoundTag.hpp>
#include <mc/Tag.hpp>
#include <mc/ByteTag.hpp>
#include <cmath>
#include "setting.h"
#include <mc/Level.hpp>
#include <mc/Block.hpp>
#include <mc/Player.hpp>
#include <mc/Actor.hpp>
#include <mc/SharedAttributes.hpp>
#include <mc/AttributeInstance.hpp>
#include <RemoteCallAPI.h>
#include <mc/Scoreboard.hpp>
#include <mc/MobEffectInstance.hpp>
#include <mc/Actor.hpp>

extern bool isDragonAlive;

void AddCrystalHealList(Vec3* pos) {
    std::ifstream DataFile("plugins/FerociousEnderDragon/data/data.json");
    if (!DataFile.is_open()) {
        return;
    }
    nlohmann::json data;
    DataFile >> data;
    DataFile.close();
    auto i = data["CrystalHeal"].size();
    data["CrystalHeal"][i] = {
        {"X", ((*pos).x)},
        {"Y", ((*pos).y)},
        {"Z", ((*pos).z)},
        {"HealTime", 0}
    };
    std::ofstream NDataFile("plugins/FerociousEnderDragon/data/data.json");
    if (!NDataFile.is_open()) {
        return;
    }
    NDataFile << data.dump(4);
    NDataFile.close();
}

void CheckAlive() {
    std::ifstream DataFile("plugins/FerociousEnderDragon/data/data.json");
    if (!DataFile.is_open()) {
        return;
    }
    nlohmann::json data;
    DataFile >> data;
    DataFile.close();
    if (data["isDragonAlive"].get<bool>() == true) {
        isDragonAlive = true;
    }
}

void SaveAlive() {
    nlohmann::json data;
    data["isDragonAlive"]= true;
    std::ofstream NDataFile("plugins/FerociousEnderDragon/data/data.json");
    if (!NDataFile.is_open()) {
        return;
    }
    NDataFile << data.dump(4);
    NDataFile.close();
}

void SaveDeath() {
    nlohmann::json data;
    data["isDragonAlive"]= false;
    std::ofstream NDataFile("plugins/FerociousEnderDragon/data/data.json");
    if (!NDataFile.is_open()) {
        return;
    }
    NDataFile << data.dump(4);
    NDataFile.close();
}

void HealCrystal(Vec3 pos) {
    ActorDefinitionIdentifier identifier("ender_crystal");
    auto en = Global<Level>->getSpawner().spawnProjectile(*Level::getBlockSource(2), identifier, nullptr, pos, pos);
    auto nbt = en->getNbt().release();
    auto tg = ByteTag::create(1);
    nbt->put("ShowBottom", std::move(tg));
    en->setNbt(nbt);
}

void CrystalHeal() {
    Schedule::repeat([](){
        std::ifstream DataFile("plugins/FerociousEnderDragon/data/data.json");
        if (!DataFile.is_open()) {
            return;
        }
        nlohmann::json data;
        DataFile >> data;
        DataFile.close();
        auto sz = data["CrystalHeal"].size();
        if (sz >= 1) {
			for (int i = 0; i <= sz-1; i++) {
                if (data["CrystalHeal"][i]["Y"].get<float>() > 0) {
                    auto healtime = data["CrystalHeal"][i]["HealTime"].get<int>() + 5;
                    if (healtime > Settings::HealCostTime) {
                        Vec3 healpos = {
                            data["CrystalHeal"][i]["X"].get<float>(),
                            data["CrystalHeal"][i]["Y"].get<float>(),
                            data["CrystalHeal"][i]["Z"].get<float>()
                        };
                        HealCrystal(healpos);
                        data["CrystalHeal"][i]["Y"] = -1;
                        data["CrystalHeal"][i]["HealTime"] = 0;
                    }
                    else {
                        data["CrystalHeal"][i]["HealTime"] = healtime;
                    }
                }
            }
            std::ofstream NDataFile("plugins/FerociousEnderDragon/data/data.json");
            if (!NDataFile.is_open()) {
                return;
            }
            NDataFile << data.dump(4);
            NDataFile.close();
        }
    }, 100);
}

void GlobalExplode() {
    Actor* src = nullptr;
    auto ens = Global<Level>->getAllEntities();
    for (auto en : ens) {
        if (en->getDimensionId() == 2 && en->getTypeName() == "minecraft:ender_dragon") {
            src = en;
            break;
        }
    }
    int r = 10 + rand() % 30;
    for (int i = 0; i <= 18; i++) {
        float x = r*cos(20*i);
        float z = r*sin(20*i);
        float y = 64;
        Vec3 pos = {x,y,z};
        Level::createExplosion(pos, 2, src, 8, false, false);
    }
}

void GlobalLightning() {
    auto pls = Level::getAllPlayers();
    if (pls.size() == 0) {
        return;
    }
    for (auto pl : pls) {
        ActorDefinitionIdentifier identifier("lightning_bolt");
        auto pos = pl->getPosition();
        Global<Level>->getSpawner().spawnProjectile(*Level::getBlockSource(2), identifier, nullptr, pos, pos);
    }
}

void SpawnChildMobs(Vec3 pos) {
    if (Settings::ChildMobList.size() == 0) {
        return;
    }
    for (auto id : Settings::ChildMobList) {
        Global<Level>->spawnMob(pos, 2, id);
    }
}

void HealDragon() {
    auto ens = Global<Level>->getAllEntities();
    for (auto en : ens) {
        if (en->getDimensionId() == 2 && en->getTypeName() == "minecraft:ender_dragon") {
            en->heal(Settings::RegenerationEachTime);
            return;
        }
    }
}

void NatureRegeneration() {
    Schedule::repeat([](){
        HealDragon();
    }, Settings::IntervalTicks);
}

void WritePlayerData(Player* pl, int fdmg) {
    std::ifstream DataFile("plugins/FerociousEnderDragon/data/player.json");
    if (!DataFile.is_open()) {
        return;
    }
    nlohmann::json data;
    DataFile >> data;
    DataFile.close();
    auto xuid = pl->getXuid();
    auto uuid = pl->getUuid();
    auto name = pl->getRealName();
    if (data[uuid].empty()) {
        data[uuid]["Damage"] = fdmg;
        data[uuid]["Xuid"] = xuid;
        data[uuid]["Name"] = name;
        data[uuid]["Uuid"] = uuid;
    }
    else {
        data[uuid]["Damage"] = data[uuid]["Damage"].get<int>() + fdmg;
    }
    data["AllDamage"] = data["AllDamage"].get<int>() + fdmg;
    std::ofstream NDataFile("plugins/FerociousEnderDragon/data/player.json");
        if (!NDataFile.is_open()) {
        return;
    }
    NDataFile << data.dump(4);
    NDataFile.close();
}

void ResetPlayerData() {
    nlohmann::json data;
    data["AllDamage"] = 0;
    std::ofstream NDataFile("plugins/FerociousEnderDragon/data/player.json");
    if (!NDataFile.is_open()) {
        return;
    }
    NDataFile << data.dump(4);
    NDataFile.close();
}

auto LLMoneyAdd = RemoteCall::importAs<bool(xuid_t xuid, int money)>("LLMoney", "LLMoneyAdd");

void KillReward() {
    std::ifstream DataFile("plugins/FerociousEnderDragon/data/player.json");
    if (!DataFile.is_open()) {
        return;
    }
    nlohmann::json data;
    DataFile >> data;
    DataFile.close();
    for (auto db : data) {
        if (db.contains("Xuid")) {
            auto per = ((float)(db["Damage"].get<int>()))/((float)(data["AllDamage"].get<int>()));
            auto mun = (int)(per*Settings::TotalReward);
            if (Settings::UseLLMoney) {
                auto xuid = db["Xuid"].get<string>();
                LLMoneyAdd(xuid, mun);
            }
            else {
                auto uuid = mce::UUID::fromString(db["Uuid"].get<string>());
                auto score = Settings::MoneyScore;
                Scoreboard::forceModifyPlayerScore(uuid, score, mun, PlayerScoreSetFunction::Add);
            }
        }
    }
}

void RemoveCrystal() {
    auto ens = Global<Level>->getAllEntities();
    for (auto en : ens) {
        if (en->getDimensionId() == 2 && en->getTypeName() == "minecraft:ender_crystal") {
            en->remove();
        }
    }
}

void AddEffect(Player* en, int effectid, int duration, int level) {
    MobEffectInstance ins = MobEffectInstance((unsigned int)effectid, duration, level, false, true, false);
    en->addEffect(ins);
}

void AddCrystalExplodeEffect(Player* pl) {
    if (Settings::CrystalEffectId.size() == 0) {
        return;
    }
    for (auto id : Settings::CrystalEffectId) {
        AddEffect(pl, id, Settings::CrystalEffectDurationTicks, Settings::CrystalEffectAmplifier);
    }
}

void DragonUseEffect() {
    if (Settings::DragonEffectId.size() != 0) {
        auto pls = Level::getAllPlayers();
        Vec3 pos = {0,64,0};
        for (auto pl : pls) {
            if (pl->distanceTo(pos) <= 64 && pl->getDimensionId() == 2) {
                for (auto id : Settings::DragonEffectId) {
                    AddEffect(pl, id, Settings::DragonEffectDurationTicks, Settings::DragonEffectAmplifier);
                }
            }
        }
    }
}

std::string TransMsg(std::string die, std::string reason) {
    try {
		nlohmann::json msgjson;
		std::ifstream LangFile("plugins/FerociousEnderDragon/DeathMessages.json");
		if (LangFile) {
			LangFile >> msgjson;
			auto dmsg = msgjson[reason].get<string>();
			ReplaceStr(dmsg, "%1$s", die);
			return dmsg;
		}
		else {
			return reason;
		}
	}
	catch(...) {
		return reason;
	}
}

std::string ChangeMsg(std::string name, Actor* en, ActorDamageSource* ads, std::string orimsg) {
    if (orimsg.length() > 65565 || isDragonAlive == false) {
        return orimsg;
    }
    Vec3 ctr = {0,64,0};
    auto adc = ads->getCause();
    switch (adc)
    {
    case ActorDamageCause::EntityExplosion:
        if (ads->isEntitySource()) {
            if (ads->getEntity()->getTypeName() == "minecraft:ender_dragon" && en->getDimensionId() == 2 && en->distanceTo(ctr) <= 64) {
                return TransMsg(name, "death.ferociousEnderDragon.explosion");
            }
        }
    case ActorDamageCause::Lightning:
        if (en->getDimensionId() == 2 && en->distanceTo(ctr) <= 64) {
            return TransMsg(name, "death.ferociousEnderDragon.lightning");
        }
    case ActorDamageCause::Magic:
        if (ads->isEntitySource()) {
            if (ads->getEntity()->getTypeName() == "minecraft:ender_dragon" && en->getDimensionId() == 2 && en->distanceTo(ctr) <= 64) {
                return TransMsg(name, "death.ferociousEnderDragon.dragonBreath");
            }
        }
    case ActorDamageCause::EntityAttack:
        if (ads->isEntitySource()) {
            if (ads->getEntity()->getTypeName() == "minecraft:ender_dragon" && en->getDimensionId() == 2 && en->distanceTo(ctr) <= 64) {
                return TransMsg(name, "death.ferociousEnderDragon.collision");
            }
        }
    case ActorDamageCause::All:
        if (ads->isEntitySource() == false) {
            if (en->getDimensionId() == 2 && en->distanceTo(ctr) <= 64) {
                return TransMsg(name, "death.ferociousEnderDragon.sonicBoom");
            }
        }
    default:
        return orimsg;
    }
}