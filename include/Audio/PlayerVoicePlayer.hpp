#pragma once

#include "custom-types/shared/macros.hpp"
#include "System/IDisposable.hpp"
#include "Zenject/ITickable.hpp"
#include "BeatSaber/AvatarCore/MultiplayerAvatarAudioController.hpp"

#include "UnityEngine/AudioSource.hpp"
#include "UnityEngine/AudioClip.hpp"

#include "FifoFloatStream.hpp"
#include "ExtendedAudioSource.hpp"

DECLARE_CLASS_CODEGEN_INTERFACES(MultiplayerChat::Audio, PlayerVoicePlayer, Il2CppObject, std::vector<Il2CppClass*>({classof(System::IDisposable*)}),
    DECLARE_INSTANCE_FIELD_PRIVATE(StringW, _playerUserId);
    DECLARE_INSTANCE_FIELD_PRIVATE(bool, _isPlaying);
    DECLARE_INSTANCE_FIELD_PRIVATE(int, _jitterBufferMs);

    DECLARE_INSTANCE_FIELD_PRIVATE(float, _spatialBlend);
    DECLARE_INSTANCE_FIELD_PRIVATE(ExtendedAudioSource*, _audioSource);
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::AudioClip*, _audioClip);
    DECLARE_INSTANCE_FIELD_PRIVATE(ArrayW<float>, _playbackBuffer);

    DECLARE_INSTANCE_FIELD_PRIVATE(bool, _havePendingFragments);
    DECLARE_INSTANCE_FIELD_PRIVATE(bool, _isJitterBuffering);
    DECLARE_INSTANCE_FIELD_PRIVATE(float, _jitterStartTime);
    DECLARE_INSTANCE_FIELD_PRIVATE(bool, _isWritingBuffer);
    DECLARE_INSTANCE_FIELD_PRIVATE(int, _lastPlaybackPos);
    DECLARE_INSTANCE_FIELD_PRIVATE(int, _playbackIterations);
    DECLARE_INSTANCE_FIELD_PRIVATE(int, _bufferPos);
    DECLARE_INSTANCE_FIELD_PRIVATE(int, _bufferIterations);
    DECLARE_INSTANCE_FIELD_PRIVATE(bool, _transmissionEnding);
    DECLARE_INSTANCE_FIELD_PRIVATE(bool, _transmissionEnded);
    DECLARE_INSTANCE_FIELD_PRIVATE(int, _deadFrames);

    DECLARE_OVERRIDE_METHOD_MATCH(void, Dispose, &::System::IDisposable::Dispose);

    DECLARE_CTOR(ctor, StringW playerUserId, int jitterBufferMs, float spatialBlend);

    public:
        void Update();
        void StopImmediate();
        void FeedFragment(ArrayW<float> decodeBuffer, int decodedLength);

        void SetMultiplayerAvatarAudioController(BeatSaber::AvatarCore::MultiplayerAvatarAudioController* avatarAudio);
        void ConfigureAudioSource(UnityEngine::AudioSource* audioSource);

        UnorderedEventCallback<PlayerVoicePlayer*> startBufferingEvent;
        UnorderedEventCallback<PlayerVoicePlayer*> startPlaybackEvent;
        UnorderedEventCallback<PlayerVoicePlayer*> stopPlaybackEvent;

        StringW get_playerUserId() const { return _playerUserId; }
        bool get_isPlaying() const { return _isPlaying; }
        int get_jitterBufferMs() const { return _jitterBufferMs; }

        static int get_clipSampleSize();
        static int get_clipFeedSize();
        static ArrayW<float> get_emptyClipSamples();
    private:
        FifoFloatStream _streamBuffer;
        void StartPlayback();

        void UpdateInactive();
        void UpdateActive(bool firstUpdate = false);

        static inline int GetAbsoluteSamples(int iterations, int samples) {
            auto clipSampleSize = get_clipSampleSize();
            while (samples >= clipSampleSize) {
                samples -= clipSampleSize;
                iterations++;
            }
            return (clipSampleSize * iterations) + samples;
        }
)
