#include "Installers/MultiplayerInstaller.hpp"

#include "lapiz/shared/utilities/ZenjectExtensions.hpp"
#include "Zenject/DiContainer.hpp"
#include "Zenject/FromBinderNonGeneric.hpp"

#include "Core/GameplayIntegrator.hpp"
#include "UI/Hud/HudVoiceIndicator.hpp"

DEFINE_TYPE(MultiplayerChat::Installers, MultiplayerInstaller);

using namespace Lapiz::Zenject::ZenjectExtensions;
namespace MultiplayerChat::Installers {
    void MultiplayerInstaller::InstallBindings() {
        auto container = get_Container();

        container->BindInterfacesAndSelfTo<Core::GameplayIntegrator*>()->AsSingle();
        FromNewComponentOnNewGameObject(container->BindInterfacesAndSelfTo<UI::Hud::HudVoiceIndicator*>())->AsSingle()->NonLazy();
    }
}
