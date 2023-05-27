#include "Core/LobbyIntegrator.hpp"
#include "config.hpp"
#include "custom-types/shared/delegate.hpp"

#include "HMUI/HoverHint.hpp"
#include "HMUI/ImageView.hpp"
#include "HMUI/ButtonSpriteSwapToggle.hpp"
#include "UnityEngine/Events/UnityAction.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/UI/Button_ButtonClickedEvent.hpp"

DEFINE_TYPE(MultiplayerChat::Core, LobbyIntegrator);

template<typename T, typename U, typename V = System::Collections::Generic::Dictionary_2<U, T>*>
static inline bool TryGetValue(V dict, T key, U& out) {
    if (dict->ContainsKey(key)) {
        out = dict->get_Item(key);
        return true;
    }
    return false;
}

namespace MultiplayerChat::Core {
    LobbyIntegrator* LobbyIntegrator::instance;
    LobbyIntegrator* LobbyIntegrator::get_instance() { return instance; }
    void LobbyIntegrator::ctor(Zenject::DiContainer* diContainer, ChatManager* chatManager, Audio::VoiceManager* voiceManager, HMUI::HoverHintController* hoverHintController, Audio::SoundNotifier* soundNotifier, UI::Lobby::ChatViewController* chatViewController, GlobalNamespace::GameServerLobbyFlowCoordinator* lobbyFlowCoordinator, GlobalNamespace::GameplaySetupViewController* gameplaySetupViewController, GlobalNamespace::ServerPlayerListViewController* serverPlayerListViewController) {
        INVOKE_CTOR();
        instance = this;

        _playerAvatars = PlayerAvatarDict::New_ctor(10);
        _playerListButtons = ButtonDict::New_ctor(10);
        _perUserBubbles = BubbleDict::New_ctor(10);

        _diContainer = diContainer;
        _chatManager = chatManager;
        _voiceManager = voiceManager;
        _hoverHintController = hoverHintController;
        _soundNotifier = soundNotifier;
        _chatViewController = chatViewController;
        _lobbyFlowCoordinator = lobbyFlowCoordinator;
        _gameplaySetupViewController = gameplaySetupViewController;
        _serverPlayerListViewController = serverPlayerListViewController;
    }

    void LobbyIntegrator::Initialize() {
        auto mainScreen = UnityEngine::GameObject::Find("Wrapper/MenuCore/UI/ScreenSystem/ScreenContainer/MainScreen");
        _centerBubble = UI::ChatBubble::Create(_diContainer, mainScreen->get_transform(), UI::AlignStyle::CenterScreen);

        _chatManager->chatClearEvent += {&LobbyIntegrator::HandleChatClear, this};
        _chatManager->chatMessageEvent += {&LobbyIntegrator::HandleChatMessage, this};
        _chatManager->chatPlayerUpdateEvent += {&LobbyIntegrator::HandleChatPlayerUpdate, this};

        _chatTitleButton = UI::Lobby::ChatButton::Create(_diContainer);
        _chatTitleButton->get_gameObject()->SetActive(false);
        _chatTitleButton->onClick += {&LobbyIntegrator::HandleChatTitleButtonClick, this};
    }

    void LobbyIntegrator::Dispose() {
        if (instance == this) instance = nullptr;

        _chatManager->chatClearEvent -= {&LobbyIntegrator::HandleChatClear, this};
        _chatManager->chatMessageEvent -= {&LobbyIntegrator::HandleChatMessage, this};
        _chatManager->chatPlayerUpdateEvent -= {&LobbyIntegrator::HandleChatPlayerUpdate, this};

        _playerAvatars->Clear();
        _playerListButtons->Clear();

        if (_centerBubble && _centerBubble->m_CachedPtr.m_value)
            UnityEngine::Object::Destroy(_centerBubble);
        _centerBubble = nullptr;

        auto enumerator = _perUserBubbles->GetEnumerator();
        while (enumerator.MoveNext()) {
            auto bubble = enumerator.get_Current().get_Value();
            if (bubble && bubble->m_CachedPtr.m_value)
                UnityEngine::Object::Destroy(bubble);
        }
        enumerator.Dispose();
        _perUserBubbles->Clear();

        if (_chatTitleButton && _chatTitleButton->m_CachedPtr.m_value) {
            _chatTitleButton->onClick -= {&LobbyIntegrator::HandleChatTitleButtonClick, this};
            UnityEngine::Object::Destroy(_chatTitleButton);
        }

        _chatTitleButton = nullptr;
    }

#pragma region Text chat events
    void LobbyIntegrator::HandleChatClear() {
        auto enumerator = _perUserBubbles->GetEnumerator();
        while (enumerator.MoveNext()) {
            auto [key, userBubble] = enumerator.get_Current();
            if (userBubble && userBubble->m_CachedPtr.m_value)
                userBubble->HideImmediate();
        }
        enumerator.Dispose();

        if (_centerBubble && _centerBubble->m_CachedPtr.m_value)
            _centerBubble->HideImmediate();

        _chatViewController->ClearMessages();

        _chatTitleButton->HideUnread();
    }

    void LobbyIntegrator::HandleChatMessage(const Models::ChatMessage& message) {
        // Player bubble
        auto showPlayerBubble = config.enablePlayerBubbles && !message.get_senderIsHost() && !message.get_senderIsMe();
        UI::ChatBubble* userBubble = nullptr;
        if (showPlayerBubble && TryGetValue(_perUserBubbles, StringW(message.get_userId()), userBubble) && userBubble && userBubble->m_CachedPtr.m_value) {
            if (userBubble->get_isShowing())
                userBubble->HideImmediate();

            userBubble->Show(message.FormatMessage(true));
        }

        // Center bubble
        auto showCenterBubble = config.enableCenterBubbles && !message.get_senderIsMe();

        if (showCenterBubble && _centerBubble && _centerBubble->m_CachedPtr.m_value) {
            if (_centerBubble->get_isShowing())
                _centerBubble->HideImmediate();

            _centerBubble->Show(message.FormatMessage());
        }

        // Notification sound
        if (!message.get_senderIsMe() && message.get_type() == Models::ChatMessageType::PlayerMessage)
            _soundNotifier->Play();

        // Chat view
        _chatViewController->AddMessage(message);

        // Unread badge
        if (!message.get_senderIsMe() && message.get_type() == Models::ChatMessageType::PlayerMessage && !_chatViewController->get_isActivated())
            _chatTitleButton->ShowUnread();
    }

    void LobbyIntegrator::HandleChatPlayerUpdate(const Models::ChatPlayer& player) {
        UpdatePlayerListState(player.get_userId());
    }

#pragma endregion

#pragma region Player list
    void LobbyIntegrator::PostfixPlayerCellSetData(GlobalNamespace::IConnectedPlayer* player, UnityEngine::UI::Button* mutePlayerButton) {
        _playerListButtons->set_Item(player->get_userId(), mutePlayerButton);

        mutePlayerButton->get_gameObject()->SetActive(true);

        auto spriteSwap = mutePlayerButton->GetComponent<HMUI::ButtonSpriteSwapToggle*>();
        if (spriteSwap) {
            if (!_nativeIconSpeakerSound || !_nativeIconSpeakerSound->m_CachedPtr.m_value)
                _nativeIconSpeakerSound = spriteSwap->normalStateSprite;

            if (!_nativeIconMuted || !_nativeIconMuted->m_CachedPtr.m_value)
                _nativeIconMuted = spriteSwap->pressedStateSprite;

            spriteSwap->set_enabled(false);
        }

        auto onClick = UnityEngine::UI::Button::ButtonClickedEvent::New_ctor();
        onClick->AddListener(custom_types::MakeDelegate<UnityEngine::Events::UnityAction*>(std::function<void()>(
            std::bind(&LobbyIntegrator::HandleMuteToggleClick, this, std::string(player->get_userId()))
        )));
        mutePlayerButton->set_onClick(onClick);
    }

    void LobbyIntegrator::HandleMuteToggleClick(std::string userId) {
        Models::ChatPlayer* chatPlayer = nullptr;
        _chatManager->TryGetChatPlayer(userId, chatPlayer);
        if (!chatPlayer) return;

        chatPlayer->isMuted = !chatPlayer->isMuted;
        _chatManager->SetIsPlayerMuted(userId, chatPlayer->isMuted);

        UpdatePlayerListState(userId, chatPlayer);

        _hoverHintController->HideHintInstant();

        if (!chatPlayer->isMuted) return;

        UI::ChatBubble* userBubble = nullptr;
        if (TryGetValue(_perUserBubbles, StringW(userId), userBubble) && userBubble)
            userBubble->HideAnimated();

        _voiceManager->HandlePlayerMuted(userId);
    }

    void LobbyIntegrator::UpdatePlayerListState(std::string userId) {
        Models::ChatPlayer* chatPlayer = nullptr;
        _chatManager->TryGetChatPlayer(userId, chatPlayer);
        UpdatePlayerListState(userId, chatPlayer);
    }

    void LobbyIntegrator::UpdatePlayerListState(std::string userId, const Models::ChatPlayer* chatPlayer) {
        UnityEngine::UI::Button* muteButton = nullptr;
        if (!TryGetValue(_playerListButtons, StringW(userId), muteButton)) return;

        auto muteButtonIcon = muteButton->get_transform()->Find("Icon")->GetComponent<HMUI::ImageView*>();

        auto hoverHint = muteButton->GetComponent<HMUI::HoverHint*>();
        if (!hoverHint)
            hoverHint = _diContainer->InstantiateComponent<HMUI::HoverHint*>(muteButton->get_gameObject());

        if (!chatPlayer || !config.enableVoiceChat) {
            // This player has not sent their capabilities, they are not using this mod
            //  (or voice chat is disabled and we don't care)

            hoverHint->set_text("Not connected to chat");
            muteButtonIcon->set_sprite(_nativeIconMuted);
            muteButtonIcon->set_color({0.5f, 0.5f, 0.5f, 1.0f});
            muteButton->set_interactable(false);
            return;
        }

        bool playerIsMe = chatPlayer->get_isMe();
        muteButton->set_interactable(!playerIsMe);

        if (chatPlayer->isMuted) {
            // this player is muted by us
            hoverHint->set_text(playerIsMe ? "Muted" : "Click to unmute");
            muteButtonIcon->set_sprite(_nativeIconMuted);
            muteButtonIcon->set_color({1, 0, 0, 1});
            return;
        }

        if (chatPlayer->isSpeaking) {
            // player is speaking and can be muted
            hoverHint->set_text(playerIsMe ? "Speaking" : "Speaking (click to mute)");
            muteButtonIcon->set_sprite(_nativeIconSpeakerSound);
            muteButtonIcon->set_color({0.18f, 0.8f, 0.443f, 1});
        } else {
            // player is idle and can be muted
            hoverHint->set_text(playerIsMe ? "Connected to chat" : "Connected to chat (click to mute)");
            muteButtonIcon->set_sprite(_nativeIconSpeakerSound);
            muteButtonIcon->set_color({1, 1, 1, 1});
        }
    }

#pragma endregion // Player list

#pragma region Lobby avatars
    void LobbyIntegrator::PostFixLobbyAvatarAddPlayer(GlobalNamespace::IConnectedPlayer* player, PlayerAvatarDict* playerIdToAvatarMap) {
        GlobalNamespace::MultiplayerLobbyAvatarController* playerAvatarController = nullptr;
        auto userId = player->get_userId();
        if (!TryGetValue(playerIdToAvatarMap, player->get_userId(), playerAvatarController)) return;

        _playerAvatars->set_Item(userId, playerAvatarController);

        UI::ChatBubble* previousBubble = nullptr;
        if (TryGetValue(_perUserBubbles, userId, previousBubble) && previousBubble && previousBubble->m_CachedPtr.m_value)
            UnityEngine::Object::Destroy(previousBubble->get_gameObject());

        auto avatarCaption = playerAvatarController->get_transform()->Find("AvatarCaption");
        auto chatBubble = UI::ChatBubble::Create(_diContainer, avatarCaption, UI::AlignStyle::LobbyAvatar);
        _perUserBubbles->set_Item(userId, chatBubble);

        _voiceManager->ProvideAvatarAudio(playerAvatarController->GetComponent<GlobalNamespace::MultiplayerAvatarAudioController*>());
    }

#pragma endregion // Lobby avatars

#pragma region Lobby setup view
    void LobbyIntegrator::PostfixLobbySetupActivation() {
        _chatTitleButton->get_gameObject()->SetActive(true);
    }

    void LobbyIntegrator::PostfixLobbySetupDeactivation() {
        if (_chatTitleButton && _chatTitleButton->m_CachedPtr.m_value)
            _chatTitleButton->get_gameObject()->SetActive(false);
    }

    void LobbyIntegrator::HandleChatTitleButtonClick() {
        _lobbyFlowCoordinator->PresentViewController(_chatViewController, nullptr, HMUI::ViewController::AnimationDirection::Horizontal, false);
        _lobbyFlowCoordinator->SetLeftScreenViewController(_gameplaySetupViewController, HMUI::ViewController::AnimationType::None);
        _lobbyFlowCoordinator->SetRightScreenViewController(_serverPlayerListViewController, HMUI::ViewController::AnimationType::None);
        _chatTitleButton->HideUnread();
    }

    bool LobbyIntegrator::PrefixFlowCoordinatorSetTitle(HMUI::ViewController* newViewController, HMUI::ViewController::AnimationType animationType) {
        if (il2cpp_utils::try_cast<UI::Lobby::ChatViewController>(newViewController).has_value()) {
            _lobbyFlowCoordinator->ShowBackButton(true);
            _lobbyFlowCoordinator->FlowCoordinator::SetTitle("Multiplayer Chat", animationType);
            return false;
        }
        return true;
    }

#pragma endregion // Lobby setup view
}
