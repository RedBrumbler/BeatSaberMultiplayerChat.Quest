#include "hooking.hpp"
#include "logging.hpp"
#include "config.hpp"
#include "assets.hpp"

#include "custom-types/shared/register.hpp"
#include "bsml/shared/BSML.hpp"
#include "bsml/shared/BSMLDataCache.hpp"
#include "lapiz/shared/zenject/Zenjector.hpp"

#include "Installers/AppInstaller.hpp"
#include "Installers/MenuInstaller.hpp"
#include "Installers/MultiplayerInstaller.hpp"

ModInfo modInfo{MOD_ID, VERSION};

Logger& getLogger() {
    static auto logger = new Logger(modInfo, LoggerOptions(false, true));
    return *logger;
}

extern "C" void setup(ModInfo& info) {
    info = modInfo;
}

extern "C" void load() {
    if (!LoadConfig())
        SaveConfig();
    il2cpp_functions::Init();

    auto& logger = getLogger();

    custom_types::Register::AutoRegister();
    Hooks::InstallHooks(logger);
    BSML::Init();

    auto zenjector = Lapiz::Zenject::Zenjector::Get();

    zenjector->Install<MultiplayerChat::Installers::AppInstaller*>(Lapiz::Zenject::Location::App);
    zenjector->Install<MultiplayerChat::Installers::MenuInstaller*>(Lapiz::Zenject::Location::Menu);
    zenjector->Install<MultiplayerChat::Installers::MultiplayerInstaller*>(Lapiz::Zenject::Location::AlwaysMultiPlayer);
}

BSML_DATACACHE(keyboard) { return IncludedAssets::Keyboard_png; }
