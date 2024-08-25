#include "Core/GameplayIntegrator.hpp"
#include "custom-types/shared/delegate.hpp"

#include "UnityEngine/Transform.hpp"
#include "UnityEngine/GameObject.hpp"
#include "BeatSaber/AvatarCore/MultiplayerAvatarAudioController.hpp"

DEFINE_TYPE(MultiplayerChat::Core, GameplayIntegrator);

namespace MultiplayerChat::Core {
    void GameplayIntegrator::ctor(GlobalNamespace::IMultiplayerSessionManager* sessionManager, GlobalNamespace::MultiplayerPlayersManager* playersManager, Audio::VoiceManager* voiceManager) {
        _sessionManager = sessionManager;
        _playersManager = playersManager;
        _voiceManager = voiceManager;
    }

    void GameplayIntegrator::Initialize() {
        _playersSpawnedAction = custom_types::MakeDelegate<std::decay_t<decltype(_playersSpawnedAction)>>(std::function<void()>(
            std::bind(&GameplayIntegrator::HandlePlayersSpawned, this)
        ));

        _playersManager->add_playerSpawningDidFinishEvent(_playersSpawnedAction);

        if (_playersManager->get_playerSpawningFinished())
            HandlePlayersSpawned();
    }

    void GameplayIntegrator::Dispose() {
        _playersManager->remove_playerSpawningDidFinishEvent(_playersSpawnedAction);
    }

    void GameplayIntegrator::HandlePlayersSpawned() {
        for (auto player : ListW<GlobalNamespace::IConnectedPlayer*>(_sessionManager->get_connectedPlayers())) {
            GlobalNamespace::MultiplayerConnectedPlayerFacade* playerController = nullptr;
            if (!_playersManager->TryGetConnectedPlayerController(player->get_userId(), byref(playerController))) continue;

            auto multiplayerGameAvatar = playerController->get_transform()->Find("MultiplayerGameAvatar");
            _voiceManager->ProvideAvatarAudio(multiplayerGameAvatar->GetComponent<BeatSaber::AvatarCore::MultiplayerAvatarAudioController*>());
        }
    }
}
