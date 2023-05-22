#pragma once

#include "custom-types/shared/macros.hpp"
#include "lapiz/shared/macros.hpp"

#include "Zenject/IInitializable.hpp"
#include "Zenject/ITickable.hpp"
#include "System/IDisposable.hpp"

#include "UnityOpus/Encoder.hpp"
#include "UnityOpus/Decoder.hpp"
#include "UnityOpus/NumChannels.hpp"
#include "UnityOpus/SamplingFrequency.hpp"
#include "UnityEngine/AudioSource.hpp"
#include "GlobalNamespace/DisconnectedReason.hpp"
#include "GlobalNamespace/IConnectedPlayer.hpp"
#include "GlobalNamespace/IMultiplayerSessionManager.hpp"
#include "GlobalNamespace/MultiplayerAvatarAudioController.hpp"
#include "System/Collections/Generic/Dictionary_2.hpp"
#include "System/Action_1.hpp"
#include "Zenject/DiContainer.hpp"

#include "multiplayer-core/shared/Networking/MpPacketSerializer.hpp"
#include "PlayerVoicePlayer.hpp"
#include "Network/MpcVoicePacket.hpp"

// Forward declares
namespace MultiplayerChat {
    namespace Core { class ChatManager; }
    namespace Audio { class MicrophoneManager; }
}

DECLARE_CLASS_CODEGEN_INTERFACES(MultiplayerChat::Audio, VoiceManager, Il2CppObject, std::vector<Il2CppClass*>({classof(System::IDisposable*), classof(Zenject::IInitializable*), classof(Zenject::ITickable*)}),
    DECLARE_INSTANCE_FIELD_PRIVATE(MultiplayerCore::Networking::MpPacketSerializer*, _packetSerializer);
    DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::IMultiplayerSessionManager*, _sessionManager);

    DECLARE_INSTANCE_FIELD_PRIVATE(System::Action_1<GlobalNamespace::DisconnectedReason>*, _disconnectedAction);
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityOpus::Encoder*, _opusEncoder);
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityOpus::Decoder*, _opusDecoder);

    DECLARE_INSTANCE_FIELD_PRIVATE(ArrayW<float>, _encodeSampleBuffer);
    DECLARE_INSTANCE_FIELD_PRIVATE(ArrayW<float>, _resampleBuffer);
    DECLARE_INSTANCE_FIELD_PRIVATE(ArrayW<uint8_t>, _encodeOutputBuffer);
    DECLARE_INSTANCE_FIELD_PRIVATE(int, _encodeSampleIndex);

    DECLARE_INSTANCE_FIELD_PRIVATE(ArrayW<float>, _decodeSampleBuffer);

    DECLARE_INSTANCE_FIELD_PRIVATE(bool, _isTransmitting);

    DECLARE_INSTANCE_FIELD_PRIVATE(bool, _isLoopbackTesting);
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::AudioSource*, _loopbackTester);

    DECLARE_INSTANCE_FIELD_PRIVATE(PlayerVoicePlayer*, _loopbackVoicePlayer);
    using VoicePlayerDict = System::Collections::Generic::Dictionary_2<StringW, PlayerVoicePlayer*>;
    DECLARE_INSTANCE_FIELD_PRIVATE(VoicePlayerDict*, _voicePlayers);

    DECLARE_OVERRIDE_METHOD(void, Tick, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::Zenject::ITickable::Tick>::get());
    DECLARE_OVERRIDE_METHOD(void, Initialize, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::Zenject::IInitializable::Initialize>::get());
    DECLARE_OVERRIDE_METHOD(void, Dispose, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::System::IDisposable::Dispose>::get());

    DECLARE_INJECT_METHOD(void, Inject, MultiplayerCore::Networking::MpPacketSerializer* packetSerializer, GlobalNamespace::IMultiplayerSessionManager* sessionManager, Zenject::DiContainer* container);
    DECLARE_CTOR(ctor);
    public:
        static constexpr const UnityOpus::NumChannels OpusChannels = UnityOpus::NumChannels::Mono;
        static constexpr const UnityOpus::SamplingFrequency OpusFrequency = UnityOpus::SamplingFrequency::Frequency_48000;
        static constexpr const int OpusComplexity = 10;
        static constexpr const int Bitrate = 96000;
        static constexpr const int FrameLength = 960; // = 20ms per voice fragment
        static constexpr const int FrameByteSize = FrameLength * sizeof(float);

        bool get_isTransmitting() const { return _isTransmitting; }
        bool get_isLoopbackTesting() const {return _isLoopbackTesting; }

        bool get_canTransmit() const;

        UnorderedEventCallback<> startedTransmittingEvent;
        UnorderedEventCallback<> stoppedTransmittingEvent;

        void HandlePlayerMuted(StringW userId);

        bool StartVoiceTransmission();
        bool StopVoiceTransmission();

        void StartLoopbackTest();
        void StopLoopbackTest();

        void ProvideAvatarAudio(GlobalNamespace::MultiplayerAvatarAudioController* avatarAudio);
    private:
        /* Circular dependencies be like: */
        Core::ChatManager* _chatManager;
        MicrophoneManager* _microphoneManager;

        void HandleSessionDisconnected(GlobalNamespace::DisconnectedReason reason);
        void HandleMicrophoneFragment(ArrayW<float> samples, int captureFrequency);
        void HandleMicrophoneEnd();
        void HandleVoicePacket(Network::MpcVoicePacket* packet, GlobalNamespace::IConnectedPlayer* source);

        UnityEngine::AudioSource* SetupLoopback();

        void HandleEncodedFrame(int encodedLength);
        void HandleVoiceFragment(int decodedLength, GlobalNamespace::IConnectedPlayer* source);
        PlayerVoicePlayer* EnsurePlayerVoicePlayer(StringW userId);

        void HandleVoicePlaybackStart(PlayerVoicePlayer* voicePlayer);
        void HandleVoicePlaybackStop(PlayerVoicePlayer* voicePlayer);
)
