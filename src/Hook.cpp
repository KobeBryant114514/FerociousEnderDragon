#include <HookAPI.h>
#include <LoggerAPI.h>
#include "Version.h"
#include <mc/DragonFireball.hpp>
#include <mc/EndDragonFight.hpp>
#include <mc/EnderDragon.hpp>
#include <mc/EnderCrystal.hpp>
#include <mc/Actor.hpp>
#include <mc/Mob.hpp>
#include <mc/Player.hpp>
#include <mc/ActorDamageSource.hpp>
#include <mc/Level.hpp>
#include <mc/BlockPos.hpp>
#include <mc/Vec3.hpp>
#include <mc/MobEffect.hpp>
#include <mc/SharedAttributes.hpp>
#include <mc/AttributeInstance.hpp>
#include "setting.h"
#include <ScheduleAPI.h>
#include <mc/ServerPlayer.hpp>
#include <mc/AreaEffectCloud.hpp>
#include "Fuctions.h"
#include <mc/CompoundTag.hpp>
#include <mc/Tag.hpp>
#include <mc/ByteTag.hpp>
#include <mc/Spawner.hpp>
#include <mc/ActorDefinitionIdentifier.hpp>
#include <mc/ItemActor.hpp>

extern Logger logger;
using namespace std;
using namespace Settings;

bool isLanding = false;
bool isDragonAlive = false;

TClasslessInstanceHook(void, "?stop@DragonTakeoffGoal@@UEAAXXZ") {
    isLanding = false;
    return original(this);
}

TClasslessInstanceHook(void, "?stop@DragonLandingGoal@@UEAAXXZ") {
    isLanding = true;
    return original(this);
}

TInstanceHook(bool, "?die@Mob@@UEAAXAEBVActorDamageSource@@@Z", Mob, ActorDamageSource* ads) {
    if (getTypeName() == "minecraft:ender_dragon") {
        isDragonAlive = false;
        SaveDeath();
        RemoveCrystal();
        KillReward();
    }
    if (isPlayer() && ads->isEntitySource() && DragonHealOnKillMob) {
        if (ads->getEntity()->getTypeName() == "minecraft:ender_dragon") {
            ads->getEntity()->heal(HealOnKillMob);
        }
    }
    return original(this, ads);
}

TInstanceHook(void, "?_setRespawnStage@EndDragonFight@@AEAAXW4RespawnAnimation@@@Z", EndDragonFight, int a1) {
    if (ModifyDragonHealth == false) {
        return original(this, a1);
    }
    original(this, a1);
    Schedule::nextTick([](){
        isDragonAlive = true;
    });
    isLanding = false;
    SaveAlive();
    ResetPlayerData();
    auto uid = dAccess<ActorUniqueID, 64>(this);
    auto en = Global<Level>->getEntity(uid);
    if (en != nullptr) {
        AttributeInstance* HP = en->getMutableAttribute(Global<SharedAttributes>->HEALTH);
        HP->setMaxValue(DragonHealth);
        HP->setCurrentValue(DragonHealth);
    }
    return;
}

TInstanceHook(bool, "?_hurt@EnderCrystal@@MEAA_NAEBVActorDamageSource@@M_N1@Z", EnderCrystal, ActorDamageSource& a1, float a2, bool a3, bool a4){
    if (AttackCrystalOnly && isDragonAlive) {
        if (a1.isEntitySource()) {
            if (a1.getEntity()->isPlayer() && a1.getCause() == ActorDamageCause::EntityAttack) {
                if (DragonLightning) {
                    ActorDefinitionIdentifier identifier("lightning_bolt");
                    auto pos = a1.getEntity()->getPosition();
                    Global<Level>->getSpawner().spawnProjectile(*Level::getBlockSource(2), identifier, nullptr, pos, pos);
                }
                if (CrystalExplodeEffect) {
                    AddCrystalExplodeEffect((Player*)(a1.getEntity()));
                }
                return original(this, a1, a2, a3, a4);
            }
        }
        return false;
    }
    else return original(this, a1, a2, a3, a4);
}

TInstanceHook(void, "?explode@Level@@UEAAXAEAVBlockSource@@PEAVActor@@AEBVVec3@@M_N3M3@Z", Level, BlockSource* bl, Actor* en, Vec3* pos, float power, bool fire, bool destory, float a2, bool a3){
	if (ModifyCrystalExplosion == false || isDragonAlive == false) {
        return original(this, bl, en, pos, power, fire, destory, a2, a3);
    }
    if (en != nullptr && isDragonAlive) {
        if (en->getTypeName() == "minecraft:ender_crystal") {
            auto nbt = en->getNbt().get()->get("ShowBottom")->asByteTag()->get();
            try{
                if ((int)nbt == 1) {
                    int Npower = CrystalExplosionPower;
                    bool Nfire = CrystalExplosionFire;
                    original(this, bl, en, pos, Npower, Nfire, destory, a2, a3);
                    if (IfCrystalHeal) {
                        AddCrystalHealList(pos);
                    }
                    return;
                }
            }
            catch (...) {}
            return original(this, bl, en, pos, power, fire, destory, a2, a3);
        }
	}
	return original(this, bl, en, pos, power, fire, destory, a2, a3);
}

TInstanceHook(bool, "?_hurt@Mob@@MEAA_NAEBVActorDamageSource@@M_N1@Z", Mob, ActorDamageSource& src, float dmg, bool a1, bool a2) {
    if (getTypeName() == "minecraft:ender_dragon") {
        Actor* source = nullptr;
        if (src.isEntitySource()) {
            if (src.isChildEntitySource()) {
                source = Level::getEntity(src.getEntityUniqueID());
            }
            else {
                source = Level::getEntity(src.getDamagingEntityUniqueID());
            }
            if (source->isPlayer()) {
                isDragonAlive = true;
                if (isLanding && ImmuneDamageWhileLanding) {
                    return false;
                }
                if (ReflectDamage) {
                    auto damage = dmg*ReflectPercentage/100;
                    source->hurtEntity(damage, ActorDamageCause::All);
                }
                if (PlayerDamageLimit && dmg >= MaxDamagePerTime) {
                    dmg = MaxDamagePerTime;
                }
                auto uid = getActorUniqueId();
                auto health = getHealth();
                auto res = original(this, src, dmg, a1, a2);
                float fdmg = 0;
                auto entity = Global<Level>->getEntity(uid);
                if (entity->getHealth() == 0) {
                    fdmg = (float)health;
                }
                else {
                    fdmg = entity->getLastHurtDamage();
                }
                WritePlayerData((Player*)source, fdmg);
                return res;
            }
            else return false;
        }
        else return false;
    }
    if (ModifyDragonDamage && isPlayer() && src.isEntitySource()) {
        auto pl = (Mob*)this;
        if (src.getEntity()->getTypeName() == "minecraft:ender_dragon") {
            isDragonAlive = true;
            if (ForceKonckback && src.getCause() == ActorDamageCause::EntityAttack) {
                auto en = (Actor*)src.getEntity();
                pl->knockback(en, 15, 8, 8, 8, 8, 8);
            }
            if (src.getCause() != ActorDamageCause::Magic) {
                dmg = DirectAttackDamage;
                return original(this, src, dmg, a1, a2);
            }
        }
    }
    if (isPlayer() && src.getCause() == ActorDamageCause::Lightning && getDimensionId() == 2 && isDragonAlive) {
        dmg = LightningDamage;
        return original(this, src, dmg, a1, a2);
    }
    return original(this, src, dmg, a1, a2);
}

TInstanceHook(float, "?getDamageAfterResistanceEffect@Mob@@UEBAMAEBVActorDamageSource@@M@Z", Mob, ActorDamageSource* src, float dmg) {
    if (getTypeName() == "minecraft:ender_dragon" && src->getCause() == ActorDamageCause::Magic) {
        return 0;
    }
    if (ModifyDragonDamage && isPlayer() && src->isEntitySource()) {
        auto pl = (Player*)this;
        if (src->getEntity()->getTypeName() == "minecraft:ender_dragon") {
            if (src->getCause() == ActorDamageCause::Magic) {
                dmg = -DragonBreathDamage;
                return original(this, src, dmg);
            }
        }
    }
    return original(this, src, dmg);
}

TClasslessInstanceHook(void, "?start@DragonFlamingGoal@@UEAAXXZ") {
    isDragonAlive = true;
    if (DragonExplosion) {
        GlobalExplode();
    }
    return original(this);
}

TClasslessInstanceHook(void, "?stop@DragonFlamingGoal@@UEAAXXZ") {
    isDragonAlive = true;
    if (DragonExplosion) {
        GlobalExplode();
    }
    return original(this);
}

TInstanceHook(int, "?getArmorValue@Mob@@UEBAHXZ", Mob) {
    if (getTypeName() == "minecraft:ender_dragon") {
        return (int)ArmorValue;
    }
    return original(this);
}

TInstanceHook(int, "?getToughnessValue@Mob@@UEBAHXZ", Mob) {
    if (getTypeName() == "minecraft:ender_dragon") {
        return (int)ToughnessValue;
    }
    return original(this);
}

TClasslessInstanceHook(void, "?start@DragonStrafePlayerGoal@@UEAAXXZ") {
    isDragonAlive = true;
    if (DragonLightning) {
        GlobalLightning();
    }
    return original(this);
}

TClasslessInstanceHook(void, "?stop@DragonStrafePlayerGoal@@UEAAXXZ") {
    isDragonAlive = true;
    if (DragonLightning) {
        GlobalLightning();
    }
    return original(this);
}

TClasslessInstanceHook(void, "?setTarget@DragonStrafePlayerGoal@@AEAAXPEAVActor@@@Z", Actor* pl) {
    isDragonAlive = true;
    if (SpawnChildMob) {
        auto pos = pl->getPosition();
        SpawnChildMobs(pos);
    }
    if (DragonExplodeEffect) {
        DragonUseEffect();
    }
    return original(this, pl);
}

TClasslessInstanceHook(DRES, "?getDeathMessage@ActorDamageSource@@UEBA?AU?$pair@V?$basic_string"
                             "@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$vector@V?$basic_string"
                             "@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$allocator@V?$basic_string"
                             "@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@2@@2@@std@@V?$basic_string"
                             "@DU?$char_traits@D@std@@V?$allocator@D@2@@3@PEAVActor@@@Z", string a1, Actor* a2) {
    auto res = original(this, a1, a2);
    auto ads = (ActorDamageSource*)this;
    res.first = ChangeMsg(a1, a2, ads, res.first);
    return res;
}

TClasslessInstanceHook(DRES, "?getDeathMessage@ActorDamageByActorSource@@UEBA?AU?$pair@V?$basic_string"
                             "@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$vector@V?$basic_string"
                             "@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$allocator@V?$basic_string"
                             "@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@2@@2@@std@@V?$basic_string"
                             "@DU?$char_traits@D@std@@V?$allocator@D@2@@3@PEAVActor@@@Z", string a1, Actor* a2) {
    auto res = original(this, a1, a2);
    auto ads = (ActorDamageSource*)this;
    res.first = ChangeMsg(a1, a2, ads, res.first);
    return res;
}

TInstanceHook(bool, "?isInvulnerableTo@ItemActor@@UEBA_NAEBVActorDamageSource@@@Z", ItemActor, ActorDamageSource* ads) {
    if (CrystalDestroyItem == false && ads->getCause() == ActorDamageCause::BlockExplosion) {
        return true;
    }
    if (ads->isEntitySource()) {
        if (DragonDestroyItem == false && ads->getEntity()->getTypeName() == "minecraft:ender_dragon") {
            return true;
        }
    }
    return original(this, ads);
}