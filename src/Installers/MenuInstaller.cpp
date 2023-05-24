#include "Installers/MenuInstaller.hpp"

#include "lapiz/shared/utilities/ZenjectExtensions.hpp"
#include "Zenject/DiContainer.hpp"
#include "Zenject/FromBinderNonGeneric.hpp"
#include "Zenject/ConcreteIdBinderGeneric_1.hpp"

#include "Core/LobbyIntegrator.hpp"
#include "UI/Hud/HudVoiceIndicator.hpp"

#include "UI/Lobby/ChatViewController.hpp"
#include "UI/ModSettings/ModSettingsViewController.hpp"
#include "UI/ModSettings/ModSettingsFlowCoordinator.hpp"
#include "UI/ModSettings/ModSettingsMenuController.hpp"

DEFINE_TYPE(MultiplayerChat::Installers, MenuInstaller);

using namespace Lapiz::Zenject::ZenjectExtensions;

namespace MultiplayerChat::Installers {
    void MenuInstaller::InstallBindings() {
        auto container = get_Container();

        // UI - Menu
        FromNewComponentAsViewController(container->Bind<UI::ModSettings::ModSettingsViewController*>())->AsSingle();
        FromNewComponentOnNewGameObject(container->Bind<UI::ModSettings::ModSettingsFlowCoordinator*>())->AsSingle();
        container->BindInterfacesAndSelfTo<UI::ModSettings::ModSettingsMenuController*>()->AsSingle();

        // UI - Lobby
        FromNewComponentAsViewController(container->Bind<UI::Lobby::ChatViewController*>())->AsSingle();

        // Core
        container->BindInterfacesAndSelfTo<Core::LobbyIntegrator*>()->AsSingle();

        // HUD
        FromNewComponentOnNewGameObject(container->BindInterfacesAndSelfTo<UI::Hud::HudVoiceIndicator*>())->AsSingle()->NonLazy();
    }
}
