#include "setting.h"

extern void CrystalHeal();
extern void AddCrystalHealList(Vec3* pos);
extern void CheckAlive();
extern void SaveAlive();
extern void GlobalExplode();
extern void GlobalLightning();
extern void SpawnChildMobs(Vec3 pos);
extern void NatureRegeneration();
extern void SaveDeath();
extern void KillReward();
extern void ResetPlayerData();
extern void WritePlayerData(Player* pl, int fdmg);
extern void RemoveCrystal();
extern void DragonUseEffect();
extern void AddCrystalExplodeEffect(Player* pl);