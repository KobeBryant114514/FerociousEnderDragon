#include "setting.h"
#include <mc/Actor.hpp>
#include <mc/ActorDamageSource.hpp>

extern void CrystalHeal();
extern void AddCrystalHealList(Vec3* pos);
extern void CheckAlive();
extern void SaveAlive();
extern void GlobalExplode();
extern void GlobalLightning();
extern void SpawnChildMobs(Vec3 pos);
extern void HealDragon();
extern void SaveDeath();
extern void KillReward();
extern void ResetPlayerData();
extern void WritePlayerData(Player* pl, int fdmg);
extern void RemoveCrystal();
extern void DragonUseEffect();
extern void AddCrystalExplodeEffect(Player* pl);
extern std::string ChangeMsg(std::string name, Actor* en, ActorDamageSource* ads, std::string orimsg);
extern void ClearList();