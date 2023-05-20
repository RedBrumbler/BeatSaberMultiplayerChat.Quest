#include "Core/ChatManager.hpp"
#include "config.hpp"
#include "logging.hpp"

#include "Network/MpcVersionInfo.hpp"

#include "custom-types/shared/delegate.hpp"

DEFINE_TYPE(MultiplayerChat::Core, ChatManager);

namespace MultiplayerChat::Core {
    void ChatManager::ctor(GlobalNamespace::IMultiplayerSessionManager* sessionManager, MultiplayerCore::Networking::MpPacketSerializer* packetSerializer, Audio::MicrophoneManager* microphoneManager, InputManager* inputManager) {
        INVOKE_CTOR();
        _sessionManager = sessionManager;
        _packetSerializer = packetSerializer;
        _microphoneManager = microphoneManager;
        _inputManager = inputManager;
    }

    void ChatManager::Initialize() {
        _localCapabilities = Network::MpcCapabilitiesPacket::New_ctor(); {
            _localCapabilities->canTextChat = get_textChatEnabled();
            _localCapabilities->canReceiveVoiceChat = get_voiceChatEnabled();
            _localCapabilities->canTransmitVoiceChat = get_voiceChatEnabled() && get_voiceChatHasValidRecordingDevice();
        }

        _sessionConnected = false;

        _sessionConnectedAction = custom_types::MakeDelegate<decltype(_sessionConnectedAction)>(std::function<void()>(
            std::bind(&ChatManager::HandleSessionConnected, this)
        ));
        _sessionDisconnectedAction = custom_types::MakeDelegate<decltype(_sessionDisconnectedAction)>(std::function<void(GlobalNamespace::DisconnectedReason)>(
            std::bind(&ChatManager::HandleSessionDisconnected, this, std::placeholders::_1)
        ));
        _sessionPlayerConnectedAction = custom_types::MakeDelegate<decltype(_sessionPlayerConnectedAction)>(std::function<void(GlobalNamespace::IConnectedPlayer*)>(
            std::bind(&ChatManager::HandleSessionPlayerConnected, this, std::placeholders::_1)
        ));
        _sessionPlayerDisconnectedAction = custom_types::MakeDelegate<decltype(_sessionPlayerDisconnectedAction)>(std::function<void(GlobalNamespace::IConnectedPlayer*)>(
            std::bind(&ChatManager::HandleSessionPlayerDisconnected, this, std::placeholders::_1)
        ));

        _sessionManager->add_connectedEvent(_sessionConnectedAction);
        _sessionManager->add_disconnectedEvent(_sessionDisconnectedAction);
        _sessionManager->add_playerConnectedEvent(_sessionPlayerConnectedAction);
        _sessionManager->add_playerDisconnectedEvent(_sessionPlayerDisconnectedAction);

        _packetSerializer->RegisterCallback<Network::MpcCapabilitiesPacket*>(std::bind(&ChatManager::HandleCapabilitiesPacket, this, std::placeholders::_1, std::placeholders::_2));
        _packetSerializer->RegisterCallback<Network::MpcTextChatPacket*>(std::bind(&ChatManager::HandleTextChat, this, std::placeholders::_1, std::placeholders::_2));
    }

    void ChatManager::Dispose() {
        _sessionManager->remove_connectedEvent(_sessionConnectedAction);
        _sessionManager->remove_disconnectedEvent(_sessionDisconnectedAction);
        _sessionManager->remove_playerConnectedEvent(_sessionPlayerConnectedAction);
        _sessionManager->remove_playerDisconnectedEvent(_sessionPlayerDisconnectedAction);

        _packetSerializer->UnregisterCallback<Network::MpcCapabilitiesPacket*>();
        _packetSerializer->UnregisterCallback<Network::MpcTextChatPacket*>();

        _sessionConnected = false;

        _chatPlayers.clear();
    }

#pragma region Text Chat API
    void ChatManager::ClearChat() {
        chatClearEvent.invoke();

        if (!get_textChatEnabled()) return;

        ShowSystemMessage(fmt::format("MultiplayerChat v{}", Network::MpcVersionInfo::SoVersion));
    }

    void ChatManager::ShowSystemMessage(std::string text) {
        if (!get_textChatEnabled() || !get_sessionConnected()) return;

        DEBUG("{}", text);

        chatMessageEvent.invoke(Models::ChatMessage::CreateSystemMessage(text));
    }

    void ChatManager::SendTextChat(std::string text) {
        if (!get_textChatEnabled() || !get_sessionConnected()) return;

        auto textPacket = Network::MpcTextChatPacket::New_ctor(); {
            textPacket->text = text;
        }

        _packetSerializer->Send(textPacket);

        chatMessageEvent.invoke(Models::ChatMessage::CreateForLocalPlayer(_sessionManager->get_localPlayer(), text));
    }

    bool ChatManager::TryGetChatPlayer(std::string userId, Models::ChatPlayer*& value) {
        auto playerItr = _chatPlayers.find(userId);
        if (playerItr != _chatPlayers.end()) {
            value = &playerItr->second;
            return true;
        }

        value = nullptr;
        return false;
    }
#pragma endregion // Text Chat API

#pragma region Voice Chat API
    void ChatManager::SetLocalPlayerIsSpeaking(bool isSpeaking) {
        auto player = _sessionManager->get_localPlayer();
        if (player)
            SetPlayerIsSpeaking(player->get_userId(), isSpeaking);
        else
            ERROR("Couldn't set local player speaking as false because the player was null!");
    }

    void ChatManager::SetPlayerIsSpeaking(std::string userId, bool isSpeaking) {
        if (userId.empty()) return;

        auto playerItr = _chatPlayers.find(userId);
        if (playerItr == _chatPlayers.end()) return;
        auto& player = playerItr->second;
        if (player.isSpeaking == isSpeaking) return;

        player.isSpeaking = isSpeaking;
        chatPlayerUpdateEvent.invoke(player);
    }

    void ChatManager::SetIsPlayerMuted(std::string userId, bool isMuted) {
        auto userIsAlreadyMuted = std::find(config.mutedUserIds.begin(), config.mutedUserIds.end(), userId) != config.mutedUserIds.end();

        if (isMuted && !userIsAlreadyMuted)
            config.mutedUserIds.emplace_back(userId);
        else if (!isMuted && userIsAlreadyMuted)
            config.mutedUserIds.remove(userId);
    }

    bool ChatManager::GetIsPlayerMuted(std::string userId) {
        return std::find(config.mutedUserIds.begin(), config.mutedUserIds.end(), userId) != config.mutedUserIds.end();
    }

#pragma endregion // Text Chat API

#pragma region Session Events
    void ChatManager::HandleSessionConnected() {
        _sessionConnected = true;

        ClearChat();
        ShowSystemMessage(fmt::format("Connected to {}", DescribeServerName()));

        auto localPlayer = _sessionManager->get_localPlayer();

        auto [idUserPair, _] = _chatPlayers.emplace(std::string(localPlayer->get_userId()), Models::ChatPlayer{localPlayer, _localCapabilities});
        auto& [userId, localChatPlayer] = *idUserPair;
        localChatPlayer.isMuted = GetIsPlayerMuted(userId);
        chatPlayerUpdateEvent.invoke(localChatPlayer);

        _packetSerializer->Send(_localCapabilities);

        _inputManager->set_testMode(false);
        _inputManager->SetActive(true);
    }

    void ChatManager::HandleSessionDisconnected(GlobalNamespace::DisconnectedReason reason) {
        _sessionConnected = false;

        ClearChat();

        _chatPlayers.clear();

        _inputManager->SetActive(false);
    }

    void ChatManager::HandleSessionPlayerConnected(GlobalNamespace::IConnectedPlayer* player) {
        if (!get_sessionConnected()) return;
        _sessionManager->SendToPlayer(_localCapabilities->i_INetSerializable(), player);
    }

    void ChatManager::HandleSessionPlayerDisconnected(GlobalNamespace::IConnectedPlayer* player) {
        if (!get_sessionConnected()) return;

        auto itr = _chatPlayers.find(player->get_userId());
        if (itr != _chatPlayers.end()) {
            _chatPlayers.erase(itr);
        } else {
            ERROR("Could not remove player with user Id '{}' because it was not in the player dict", player->get_userId());
        }
    }

#pragma endregion // Session Events

#pragma region Packet Handlers
    void ChatManager::HandleCapabilitiesPacket(Network::MpcCapabilitiesPacket* packet, GlobalNamespace::IConnectedPlayer* sender) {
        if (!get_sessionConnected()) return;

        DEBUG("Received capabilities (userId={}, protoVersion={}, canText={}, canVoice={})",
            sender->get_userId(), packet->protocolVersion, packet->canTextChat, packet->canTransmitVoiceChat
        );

        auto prevChatPlayer = _chatPlayers.find(sender->get_userId());
        bool isNewEntry = prevChatPlayer == _chatPlayers.end();

        Models::ChatPlayer chatPlayer(sender, packet);
        chatPlayer.isMuted = GetIsPlayerMuted(chatPlayer.get_userId());

        if (!isNewEntry) {
            chatPlayer.isTyping = prevChatPlayer->second.isTyping;
            chatPlayer.isSpeaking = prevChatPlayer->second.isSpeaking;
            _chatPlayers.erase(prevChatPlayer);
        }

        auto [emplaceResult, _] = _chatPlayers.emplace(sender->get_userId(), chatPlayer);

        if (isNewEntry) {
            if (packet->canTextChat)
                ShowSystemMessage(fmt::format("Player Connected to chat: {}", sender->get_userName()));
            if (packet->protocolVersion > Network::MpcVersionInfo::ProtocolVersion)
                ShowSystemMessage(fmt::format("Player {} is using a newer version of MultiplayerChat. You should update for the best experience", sender->get_userName()));
        }

        chatPlayerUpdateEvent.invoke(emplaceResult->second);
    }

    void ChatManager::HandleTextChat(Network::MpcTextChatPacket* packet, GlobalNamespace::IConnectedPlayer* sender) {
        if (!get_sessionConnected() || !get_textChatEnabled()) return;

        if (GetIsPlayerMuted(sender->get_userId())) return;

        chatMessageEvent.invoke(Models::ChatMessage::CreateFromPacket(sender, packet));
    }

#pragma endregion // Packet Handlers

#pragma region Utils
    std::string ChatManager::DescribeServerName() {
        if (_sessionManager->get_connectionOwner() && !Il2CppString::IsNullOrWhiteSpace(_sessionManager->get_connectionOwner()->get_userName())) {
            return _sessionManager->get_connectionOwner()->get_userName();
        }

        return "Dedicated Server";
    }

#pragma endregion // Utils

    bool ChatManager::get_textChatEnabled() const { return config.enableTextchat; }

    bool ChatManager::get_voiceChatEnabled() const { return config.enableVoiceChat; }

    bool ChatManager::get_voiceChatHasValidRecordingDevice() const {
        return get_voiceChatEnabled() && _microphoneManager->get_haveSelectedDevice();
    }
}
