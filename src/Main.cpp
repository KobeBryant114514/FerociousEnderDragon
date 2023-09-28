#include <LoggerAPI.h>
#include "version.h"
#include "setting.h"
#include "Fuctions.h"
#include <EventAPI.h>
#include <mc/Scoreboard.hpp>
#include <ScheduleAPI.h>
#include "GMLib/GMLib_ModAPI.h"

extern Logger logger;
extern bool isDragonAlive;
extern std::vector<unsigned char> RP_Data;

int CrystalTimer = 0;
int HealTimer = 0;

void PluginInit()
{
    Logger(PLUGIN_NAME).info("Loaded Version {}.{}.{}",PLUGIN_VERSION_MAJOR,PLUGIN_VERSION_MINOR,PLUGIN_VERSION_REVISION);
    Logger(PLUGIN_NAME).info("Author Tsubasa6848");
    GMLib_Mod::addResourcePack("FerociousEnderDragon", RP_Data, "1.0.0");
    GMLib_Mod::addDamageCause(ActorDamageCause::Magic, "dragonBreath", "minecraft:ender_dragon");
    GMLib_Mod::addDamageCause(ActorDamageCause::EntityAttack, "dragonAttack", "minecraft:ender_dragon");
    GMLib_Mod::addDamageCause(ActorDamageCause::EntityExplosion, "dragonExplode", "minecraft:ender_dragon");
    GMLib_Mod::addDamageCause(ActorDamageCause::All, "dragonThorns", "minecraft:ender_dragon");
    loadConfig();
    CheckAlive();
    Event::ServerStartedEvent::subscribe([](const Event::ServerStartedEvent){
        if (Settings::EnableReward && Settings::UseLLMoney == true) {
            auto llmoney = ll::getPlugin("LLMoney");
            if (!llmoney) {
                logger.error("LLMoney not found! Economy system will not work");
            }
        }
        if (Settings::EnableReward && Settings::UseLLMoney == false) {
            auto obj = Global<Scoreboard>->getObjective(Settings::MoneyScore);
            if (!obj) {
                Global<Scoreboard>->newObjective(Settings::MoneyScore, Settings::MoneyScore);
            }
        }
        return true;
    });
    Schedule::repeat([](){
        CrystalTimer++;
        HealTimer ++;
        if (CrystalTimer >= 100) {
            if (CrystalTimer == 100) {
                try {
                    CrystalHeal();
                }
                catch (...) {}   
            }
            CrystalTimer = 0;
        }
        if (HealTimer >= Settings::IntervalTicks) {
            if (HealTimer == Settings::IntervalTicks) {
                try {
                    HealDragon();
                }
                catch (...) {}   
            }
            HealTimer = 0;
        }
    }, 1);
    std::thread([](){
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(2));
            ClearList();
        }
    }).detach();
}