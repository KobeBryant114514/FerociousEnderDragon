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

extern bool isDragonAlive;

void AddCrystalHealList(Vec3* pos) {
    std::ifstream DataFile("plugins/FerociousEnderDragon/data.json");
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
    std::ofstream NDataFile("plugins/FerociousEnderDragon/data.json");
    if (!NDataFile.is_open()) {
        return;
    }
    NDataFile << data.dump(4);
    NDataFile.close();
}

void CheckAlive() {
    std::ifstream DataFile("plugins/FerociousEnderDragon/data.json");
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
    std::ofstream NDataFile("plugins/FerociousEnderDragon/data.json");
    if (!NDataFile.is_open()) {
        return;
    }
    NDataFile << data.dump(4);
    NDataFile.close();
}

void SaveDeath() {
    nlohmann::json data;
    data["isDragonAlive"]= false;
    std::ofstream NDataFile("plugins/FerociousEnderDragon/data.json");
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
        std::ifstream DataFile("plugins/FerociousEnderDragon/data.json");
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
            std::ofstream NDataFile("plugins/FerociousEnderDragon/data.json");
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
    auto ens = Global<Level>->getAllEntities(2);
    for (auto en : ens) {
        if (en->getTypeName() == "minecraft:ender_dragon") {
            src = en;
            break;
        }
    }
    int r = 10 + rand() % 30;   //r = 10 ~ 40
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