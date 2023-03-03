#include <LoggerAPI.h>
#include "version.h"
#include "setting.h"
#include "Fuctions.h"
#include <EventAPI.h>
#include <mc/Scoreboard.hpp>

extern Logger logger;
extern bool isDragonAlive;

void PluginInit()
{
    Logger(PLUGIN_NAME).info("Loaded Version {}.{}.{}",PLUGIN_VERSION_MAJOR,PLUGIN_VERSION_MINOR,PLUGIN_VERSION_REVISION);
    Logger(PLUGIN_NAME).info("Author Tsubasa6848");
    loadConfig();
    CheckAlive();
    Event::ServerStartedEvent::subscribe([](const Event::ServerStartedEvent){
        if (Settings::IfCrystalHeal) {
            CrystalHeal();
        }
        if (Settings::NatureRegeneration) {
            NatureRegeneration();
        }
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
}