#include "hooking.hpp"
#include "logging.hpp"
#include "config.hpp"
#include "assets.hpp"
#include "_config.h"

#include "custom-types/shared/register.hpp"
#include "bsml/shared/BSML.hpp"
#include "bsml/shared/BSMLDataCache.hpp"
#include "lapiz/shared/zenject/Zenjector.hpp"

#include "Installers/AppInstaller.hpp"
#include "Installers/MenuInstaller.hpp"
#include "Installers/MultiplayerInstaller.hpp"

#include <fstream>

modloader::ModInfo modInfo{MOD_ID, VERSION, VERSION_LONG};

void write_bytes(std::string_view path, std::span<uint8_t> data) {
    std::ofstream file;
    file.open(path, std::ios::binary | std::ios::out);
    file.write((const char*)data.data(), data.size());
    file.close();
}

#define WRITE_SOUND(identifier) \
    INFO("Writing out sound '"  #identifier ".ogg'"); \
    write_bytes("/sdcard/ModData/com.beatgames.beatsaber/Mods/MultiplayerChat/Sounds/" #identifier ".ogg", Assets::identifier##_ogg)

void ExportSoundFiles() {
    mkpath("/sdcard/ModData/com.beatgames.beatsaber/Mods/MultiplayerChat/Sounds/");
    WRITE_SOUND(ClubPing);
    WRITE_SOUND(ClubPing2);
    WRITE_SOUND(ClubPing3);
    WRITE_SOUND(MicOn);
    WRITE_SOUND(MicOff);
}

MPCHAT_EXPORT_FUNC void setup(CModInfo* info) {
    info->id = MOD_ID;
    info->version = VERSION;
    info->version_long = VERSION_LONG;

    INFO("Mod '{}' with version '{}' version long '{}' finished setup", MOD_ID, VERSION, VERSION_LONG);
}

MPCHAT_EXPORT_FUNC void late_load() {
    if (!LoadConfig())
        SaveConfig();

    if (!config.exportedSoundFilesBefore) ExportSoundFiles();

    il2cpp_functions::Init();

    // auto& logger = getLogger();

    custom_types::Register::AutoRegister();
    // Hooks::InstallHooks(logger);
    MultiplayerChat::Hooking::InstallHooks();
    BSML::Init();

    auto zenjector = Lapiz::Zenject::Zenjector::Get();

    zenjector->Install<MultiplayerChat::Installers::AppInstaller*>(Lapiz::Zenject::Location::App);
    zenjector->Install<MultiplayerChat::Installers::MenuInstaller*>(Lapiz::Zenject::Location::Menu);
    zenjector->Install<MultiplayerChat::Installers::MultiplayerInstaller*>(Lapiz::Zenject::Location::AlwaysMultiPlayer);
}

BSML_DATACACHE(keyboard) { return Assets::Keyboard_png; }
