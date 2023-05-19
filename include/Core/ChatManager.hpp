#pragma once

#include "custom-types/shared/macros.hpp"
#include "Zenject/IInitializable.hpp"
#include "System/IDisposable.hpp"

#include "Audio/MicrophoneManager.hpp"
#include "Network/MpcCapabilitiesPacket.hpp"
#include "Network/MpcTextChatPacket.hpp"
#include "Models/ChatMessage.hpp"
#include "Models/ChatPlayer.hpp"
#include "System/Action.hpp"
#include "System/Action_1.hpp"

#include "InputManager.hpp"

#include "multiplayer-core/shared/Networking/MpPacketSerializer.hpp"

DECLARE_CLASS_CODEGEN_INTERFACES(MultiplayerChat::Core, ChatManager, Il2CppObject, std::vector<Il2CppClass*>({ classof(Zenject::IInitializable*), classof(System::IDisposable*)}),
    DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::IMultiplayerSessionManager*, _sessionManager);
    DECLARE_INSTANCE_FIELD_PRIVATE(MultiplayerCore::Networking::MpPacketSerializer*, _packetSerializer);
    DECLARE_INSTANCE_FIELD_PRIVATE(Audio::MicrophoneManager*, _microphoneManager);
    DECLARE_INSTANCE_FIELD_PRIVATE(InputManager*, _inputManager);

    DECLARE_INSTANCE_FIELD_PRIVATE(Network::MpcCapabilitiesPacket*, _localCapabilities);
    DECLARE_INSTANCE_FIELD_PRIVATE(bool, _sessionConnected);

    DECLARE_INSTANCE_FIELD_PRIVATE(System::Action*, _sessionConnectedAction);
    DECLARE_INSTANCE_FIELD_PRIVATE(System::Action_1<GlobalNamespace::DisconnectedReason>*, _sessionDisconnectedAction);
    DECLARE_INSTANCE_FIELD_PRIVATE(System::Action_1<GlobalNamespace::IConnectedPlayer*>*, _sessionPlayerConnectedAction);
    DECLARE_INSTANCE_FIELD_PRIVATE(System::Action_1<GlobalNamespace::IConnectedPlayer*>*, _sessionPlayerDisconnectedAction);

    DECLARE_OVERRIDE_METHOD(void, Initialize, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::Zenject::IInitializable::Initialize>::get());
    DECLARE_OVERRIDE_METHOD(void, Dispose, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::System::IDisposable::Dispose>::get());

    DECLARE_CTOR(ctor, GlobalNamespace::IMultiplayerSessionManager* sessionManager, MultiplayerCore::Networking::MpPacketSerializer* packetSerializer, Audio::MicrophoneManager* microphoneManager, InputManager* inputManager);
    public:
        UnorderedEventCallback<const Models::ChatMessage&> chatMessageEvent;
        UnorderedEventCallback<> chatClearEvent;
        UnorderedEventCallback<const Models::ChatPlayer&> chatPlayerUpdateEvent;

        bool get_sessionConnected() const { return _sessionConnected; }
        bool get_textChatEnabled() const;
        bool get_voiceChatEnabled() const;
        bool get_voiceChatHasValidRecordingDevice() const;

        void ClearChat();
        void ShowSystemMessage(std::string text);
        void SendTextChat(std::string text);
        bool TryGetChatPlayer(std::string userId, Models::ChatPlayer*& value);

        void SetLocalPlayerIsSpeaking(bool isSpeaking = true);
        void SetPlayerIsSpeaking(std::string userId, bool isSpeaking = true);
        void SetIsPlayerMuted(std::string userId, bool isMuted = true);
        bool GetIsPlayerMuted(std::string userId);
    private:
        std::map<std::string, Models::ChatPlayer> _chatPlayers;

        void HandleSessionConnected();
        void HandleSessionDisconnected(GlobalNamespace::DisconnectedReason reason);
        void HandleSessionPlayerConnected(GlobalNamespace::IConnectedPlayer* player);
        void HandleSessionPlayerDisconnected(GlobalNamespace::IConnectedPlayer* player);

        void HandleCapabilitiesPacket(Network::MpcCapabilitiesPacket* packet, GlobalNamespace::IConnectedPlayer* sender);
        void HandleTextChat(Network::MpcTextChatPacket* packet, GlobalNamespace::IConnectedPlayer* sender);

        std::string DescribeServerName();
)
