#include "Installers/AppInstaller.hpp"

#include "Zenject/DiContainer.hpp"
#include "Zenject/FromBinderNonGeneric.hpp"

#include "Audio/MicrophoneManager.hpp"
#include "Audio/SoundNotifier.hpp"
#include "Audio/VoiceManager.hpp"

#include "Core/ChatManager.hpp"
#include "Core/InputManager.hpp"

#include "Models/QuickChatOptions.hpp"

#include "Utilities/SpriteManager.hpp"

#include "lapiz/shared/utilities/ZenjectExtensions.hpp"
using namespace Lapiz::Zenject::ZenjectExtensions;

DEFINE_TYPE(MultiplayerChat::Installers, AppInstaller);

namespace MultiplayerChat::Installers {
    void AppInstaller::InstallBindings() {
        auto container = get_Container();

        container->BindInterfacesAndSelfTo<Utilities::SpriteManager*>()->AsSingle();

        container->BindInterfacesAndSelfTo<Audio::MicrophoneManager*>()->AsSingle();
        FromNewComponentOnNewGameObject(container->BindInterfacesAndSelfTo<Audio::SoundNotifier*>())->AsSingle();

        FromNewComponentOnNewGameObject(container->BindInterfacesAndSelfTo<Core::InputManager*>())->AsSingle();

        container->BindInterfacesAndSelfTo<Core::ChatManager*>()->AsSingle();
        container->BindInterfacesAndSelfTo<Audio::VoiceManager*>()->AsSingle();

        container->BindInterfacesAndSelfTo<Models::QuickChatOptions*>()->AsSingle();
    }
}
