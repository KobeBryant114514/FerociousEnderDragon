#include <HookAPI.h>
#include <LoggerAPI.h>
//#include <mc/DragonBaseGoal.hpp>
//#include <mc/DragonChargePlayerGoal.hpp>
#include <mc/DragonDeathGoal.hpp>
#include <mc/DragonFireball.hpp>
#include <mc/DragonFlamingGoal.hpp>
//#include <mc/DragonHoldingPatternGoal.hpp>
#include <mc/DragonLandingGoal.hpp>
#include <mc/DragonScanningGoal.hpp>
#include <mc/DragonStrafePlayerGoal.hpp>
#include <mc/DragonTakeoffGoal.hpp>
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
#include <time.h>
#include "Fuctions.h"
#include <mc/CompoundTag.hpp>
#include <mc/Tag.hpp>
#include <mc/ByteTag.hpp>
#include <mc/Spawner.hpp>
#include <mc/ActorDefinitionIdentifier.hpp>

extern Logger logger;
using namespace std;
using namespace Settings;

bool isLanding = false;
bool isDragonAlive = false;
int Interval = 0;

TInstanceHook(void, "?stop@DragonTakeoffGoal@@UEAAXXZ", DragonTakeoffGoal) {
    isLanding = false;
    return original(this);
}

TInstanceHook(void, "?stop@DragonLandingGoal@@UEAAXXZ", DragonLandingGoal) {
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
    return original(this, ads);
}

TClasslessInstanceHook(void, "?_setRespawnStage@EndDragonFight@@AEAAXW4RespawnAnimation@@@Z", int a1) {
    if (ModifyDragonHealth == false) {
        return original(this, a1);
    }
    original(this, a1);
    isDragonAlive = true;
    isLanding = false;
    SaveAlive();
    ResetPlayerData();
    auto uid = dAccess<ActorUniqueID>(this, 64);
    auto en = Global<Level>->getEntity(uid);
    AttributeInstance* HP = en->getMutableAttribute(Global<SharedAttributes>->HEALTH);
    HP->setMaxValue(DragonHealth);
    HP->setCurrentValue(DragonHealth);
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
    if (en != nullptr) {
        if (en->getTypeName() == "minecraft:ender_crystal") {
            if (en->getNbt().get()->get("ShowBottom")->toJson(4) == "1") {
                int Npower = CrystalExplosionPower;
                bool Nfire = CrystalExplosionFire;
                original(this, bl, en, pos, Npower, Nfire, destory, a2, a3);
                if (IfCrystalHeal) {
                    AddCrystalHealList(pos);
                }
                return;
            }
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
                if (isLanding && ImmuneDamageWhileLanding) {
                    return false;
                }
                if (ReflectDamage) {
                    auto damage = dmg*ReflectPercentage;
                    source->hurtEntity(damage, ActorDamageCause::All);
                }
                if (PlayerDamageLimit && dmg >= MaxDamagePerTime) { //Damage Limit
                    dmg = MaxDamagePerTime;
                }
                auto uid = getActorUniqueId();
                auto res = original(this, src, dmg, a1, a2);
                auto fdmg = Global<Level>->getEntity(uid)->getLastHurtDamage();
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
            if (ForceKonckback) {
                auto en = (Actor*)src.getEntity();
                pl->knockback(en, 30, 10, 10, 10, 10, 10);
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

TInstanceHook(void, "?start@DragonFlamingGoal@@UEAAXXZ", DragonFlamingGoal) {
    if (DragonExplosion) {
        GlobalExplode();
    }
    return original(this);
}

TInstanceHook(void, "?stop@DragonFlamingGoal@@UEAAXXZ", DragonFlamingGoal) {
    if (DragonExplosion) {
        GlobalExplode();
    }
    return original(this);
}

TInstanceHook(void, "?start@DragonStrafePlayerGoal@@UEAAXXZ", DragonStrafePlayerGoal) {
    if (DragonLightning) {
        GlobalLightning();
    }
    return original(this);
}

TInstanceHook(void, "?stop@DragonStrafePlayerGoal@@UEAAXXZ", DragonStrafePlayerGoal) {
    if (DragonLightning) {
        GlobalLightning();
    }
    return original(this);
}

TInstanceHook(void, "?setTarget@DragonStrafePlayerGoal@@AEAAXPEAVActor@@@Z", DragonStrafePlayerGoal, Actor* pl) {
    if (SpawnChildMob) {
        auto pos = pl->getPosition();
        SpawnChildMobs(pos);
    }
    if (DragonExplodeEffect) {
        DragonUseEffect();
    }
    return original(this, pl);
}