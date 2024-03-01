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

#include <fstream>

modloader::ModInfo modInfo{MOD_ID, VERSION, 0};

Logger& getLogger() {
    static auto logger = new Logger(modInfo, LoggerOptions(false, true));
    return *logger;
}

void write_bytes(std::string_view path, std::span<uint8_t> data) {
    std::ofstream file;
    file.open(path, std::ios::binary | std::ios::out);
    file.write((const char*)data.data(), data.size());
    file.close();
}

#define WRITE_SOUND(identifier) \
    INFO("Writing out sound '"  #identifier ".ogg'"); \
    write_bytes("/sdcard/ModData/com.beatgames.beatsaber/Mods/MultiplayerChat/Sounds/" #identifier ".ogg", Assets::Sounds::identifier##_ogg)

void ExportSoundFiles() {
    mkpath("/sdcard/ModData/com.beatgames.beatsaber/Mods/MultiplayerChat/Sounds/");
    WRITE_SOUND(ClubPing);
    WRITE_SOUND(ClubPing2);
    WRITE_SOUND(ClubPing3);
    WRITE_SOUND(MicOn);
    WRITE_SOUND(MicOff);
}

extern "C" __attribute__((visibility("default"))) void setup(CModInfo* info) {
    info->id = MOD_ID;
    info->version = VERSION;
    info->version_long = 0;
}

extern "C" __attribute__((visibility("default"))) void late_load() {
    if (!LoadConfig())
        SaveConfig();

    if (!config.exportedSoundFilesBefore) ExportSoundFiles();

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

BSML_DATACACHE(keyboard) { return Assets::Icons::Keyboard_png; }
