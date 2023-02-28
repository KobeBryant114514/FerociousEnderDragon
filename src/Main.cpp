#include <LoggerAPI.h>
#include "version.h"
#include "setting.h"
#include "Fuctions.h"

extern Logger logger;
extern bool isDragonAlive;

void PluginInit()
{
    Logger(PLUGIN_NAME).info("Loaded Version {}.{}.{}",PLUGIN_VERSION_MAJOR,PLUGIN_VERSION_MINOR,PLUGIN_VERSION_REVISION);
    Logger(PLUGIN_NAME).info("Author Tsubasa6848");
    loadConfig();
    CheckAlive();
    if (Settings::IfCrystalHeal) {
        CrystalHeal();
    }
    if (Settings::NatureRegeneration) {
        NatureRegeneration();
    }
}