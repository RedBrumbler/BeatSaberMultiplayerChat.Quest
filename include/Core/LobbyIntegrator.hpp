#pragma once

#include "custom-types/shared/macros.hpp"
#include "lapiz/shared/macros.hpp"

#include "Zenject/IInitializable.hpp"
#include "System/IDisposable.hpp"

#include "ChatManager.hpp"
#include "Audio/VoiceManager.hpp"
#include "Audio/SoundNotifier.hpp"
#include "UI/Lobby/ChatViewController.hpp"

#include "HMUI/HoverHintController.hpp"
#include "GlobalNamespace/GameServerLobbyFlowCoordinator.hpp"
#include "GlobalNamespace/GameplaySetupViewController.hpp"
#include "GlobalNamespace/ServerPlayerListViewController.hpp"
#include "Zenject/DiContainer.hpp"

#include "System/Collections/Generic/Dictionary_2.hpp"
#include "GlobalNamespace/MultiplayerLobbyAvatarController.hpp"
#include "GlobalNamespace/IConnectedPlayer.hpp"
#include "UnityEngine/Sprite.hpp"
#include "UnityEngine/UI/Button.hpp"
#include "HMUI/ViewController.hpp"
// #include "HMUI/ViewController_AnimationType.hpp"
#include "UI/ChatBubble.hpp"
#include "UI/Lobby/ChatButton.hpp"

struct Hook_GameServerPlayerTableCell_SetData;
struct Hook_MultiplayerLobbyAvatarManager_AddPlayer;
struct Hook_LobbySetupViewController_DidActivate;
struct Hook_ViewController_Deactivate;
struct Hook_GameServerLobbyFlowCoordinator_SetTitle;

DECLARE_CLASS_CODEGEN_INTERFACES(MultiplayerChat::Core, LobbyIntegrator, Il2CppObject, std::vector<Il2CppClass*>({ classof(Zenject::IInitializable*), classof(System::IDisposable*)}),
    DECLARE_INSTANCE_FIELD_PRIVATE(Zenject::DiContainer*, _diContainer);
    DECLARE_INSTANCE_FIELD_PRIVATE(ChatManager*, _chatManager);
    DECLARE_INSTANCE_FIELD_PRIVATE(Audio::VoiceManager*, _voiceManager);
    DECLARE_INSTANCE_FIELD_PRIVATE(HMUI::HoverHintController*, _hoverHintController);
    DECLARE_INSTANCE_FIELD_PRIVATE(Audio::SoundNotifier*, _soundNotifier);
    DECLARE_INSTANCE_FIELD_PRIVATE(UI::Lobby::ChatViewController*, _chatViewController);
    DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::GameServerLobbyFlowCoordinator*, _lobbyFlowCoordinator);
    DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::GameplaySetupViewController*, _gameplaySetupViewController);
    DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::ServerPlayerListViewController*, _serverPlayerListViewController);

    DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::Sprite*, _nativeIconSpeakerSound);
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::Sprite*, _nativeIconMuted);

    using PlayerAvatarDict = ::System::Collections::Generic::Dictionary_2<::StringW, ::UnityW<::GlobalNamespace::MultiplayerLobbyAvatarController>>;
    DECLARE_INSTANCE_FIELD_PRIVATE(PlayerAvatarDict*, _playerAvatars);
    using ButtonDict = System::Collections::Generic::Dictionary_2<StringW, UnityEngine::UI::Button*>;
    DECLARE_INSTANCE_FIELD_PRIVATE(ButtonDict*, _playerListButtons);
    DECLARE_INSTANCE_FIELD_PRIVATE(UI::ChatBubble*, _centerBubble);
    using BubbleDict = System::Collections::Generic::Dictionary_2<StringW, UI::ChatBubble*>;
    DECLARE_INSTANCE_FIELD_PRIVATE(BubbleDict*, _perUserBubbles);
    DECLARE_INSTANCE_FIELD_PRIVATE(UI::Lobby::ChatButton*, _chatTitleButton);

    DECLARE_OVERRIDE_METHOD_MATCH(void, Initialize, &::Zenject::IInitializable::Initialize);
    DECLARE_OVERRIDE_METHOD_MATCH(void, Dispose, &::System::IDisposable::Dispose);

    DECLARE_CTOR(ctor, Zenject::DiContainer* diContainer, ChatManager* chatManager, Audio::VoiceManager* voiceManager, HMUI::HoverHintController* hoverHintController, Audio::SoundNotifier* soundNotifier, UI::Lobby::ChatViewController* chatViewController, GlobalNamespace::GameServerLobbyFlowCoordinator* lobbyFlowCoordinator, GlobalNamespace::GameplaySetupViewController* gameplaySetupViewController, GlobalNamespace::ServerPlayerListViewController* serverPlayerListViewController);
    public:
        static LobbyIntegrator* get_instance();
    private:
        static LobbyIntegrator* instance;

        void HandleChatClear();
        void HandleChatMessage(const Models::ChatMessage& chatMessage);
        void HandleChatPlayerUpdate(const Models::ChatPlayer& player);

        friend Hook_GameServerPlayerTableCell_SetData;
        void PostfixPlayerCellSetData(GlobalNamespace::IConnectedPlayer* player, UnityEngine::UI::Button* mutePlayerButton);
        void HandleMuteToggleClick(std::string userId);
        void UpdatePlayerListState(std::string userId);
        void UpdatePlayerListState(std::string userId, const Models::ChatPlayer* chatPlayer);

        friend Hook_MultiplayerLobbyAvatarManager_AddPlayer;
        void PostFixLobbyAvatarAddPlayer(GlobalNamespace::IConnectedPlayer* player, PlayerAvatarDict* playerIdToAvatarMap);

        friend Hook_LobbySetupViewController_DidActivate;
        void PostfixLobbySetupActivation();
        friend Hook_ViewController_Deactivate;
        void PostfixLobbySetupDeactivation();
        void HandleChatTitleButtonClick();
        friend Hook_GameServerLobbyFlowCoordinator_SetTitle;
        bool PrefixFlowCoordinatorSetTitle(HMUI::ViewController* newViewController, HMUI::ViewController::AnimationType animationType);

)
