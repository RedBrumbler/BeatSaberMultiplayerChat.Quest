#pragma once

#include "custom-types/shared/macros.hpp"
#include "UnityEngine/AudioClip.hpp"

#include "Zenject/IInitializable.hpp"
#include "Zenject/ITickable.hpp"
#include "System/IDisposable.hpp"

#include "Microphone.hpp"

DECLARE_CLASS_CODEGEN_INTERFACES(MultiplayerChat::Audio, MicrophoneManager, Il2CppObject,  std::vector<Il2CppClass*>({classof(System::IDisposable*), classof(Zenject::IInitializable*), classof(Zenject::ITickable*)}),
    DECLARE_INSTANCE_FIELD_PRIVATE(StringW, _selectedDeviceName);
    DECLARE_INSTANCE_FIELD_PRIVATE(bool, _haveSelectedDevice);

    DECLARE_INSTANCE_FIELD_PRIVATE(int, _minFreq);
    DECLARE_INSTANCE_FIELD_PRIVATE(int, _maxFreq);
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::AudioClip*, _captureClip);

    DECLARE_INSTANCE_FIELD_PRIVATE(int, _micBufferPos);
    DECLARE_INSTANCE_FIELD_PRIVATE(ArrayW<float>, _fragmentBuffer);
    DECLARE_INSTANCE_FIELD_PRIVATE(ArrayW<float>, _micBuffer);
    DECLARE_INSTANCE_FIELD_PRIVATE(bool, _isCapturing);

    DECLARE_OVERRIDE_METHOD_MATCH(void, Tick, &::Zenject::ITickable::Tick);
    DECLARE_OVERRIDE_METHOD_MATCH(void, Initialize, &::Zenject::IInitializable::Initialize);
    DECLARE_OVERRIDE_METHOD_MATCH(void, Dispose, &::System::IDisposable::Dispose);

    DECLARE_CTOR(ctor);
    public:
        static constexpr const int TargetFrequency = 48000;
        static constexpr const int SamplesPerFragment = 512;

        UnorderedEventCallback<ArrayW<float>, int> onFragmentReadyEvent;
        UnorderedEventCallback<> onCaptureEndEvent;

        StringW get_selectedDeviceName() const { return _selectedDeviceName; }
        bool get_isCapturing() const { return _isCapturing; }
        bool get_haveSelectedDevice() const { return _haveSelectedDevice; }

        static inline bool AnyDevicesAvailable() { return Microphone::get_devices().size() > 0; }
        static ArrayW<StringW> GetAvailableDeviceNames(bool ignorePermission = false);
        static bool get_hasMicrophonePermission() {
            static ConstString microphonePermission("android.permission.RECORD_AUDIO");
            return Microphone::HasUserAuthorizedPerrmission(microphonePermission);
        }

        bool TryAutoSelectDevice();
        bool TrySelectDevice(StringW deviceName);
        inline int GetRecordingFrequency() const {
            if (_minFreq == 0 && _maxFreq == 0) return TargetFrequency;
            return std::clamp(TargetFrequency, _minFreq, _maxFreq);
        }

        void StartCapture();
        void StopCapture();
    private:
        static inline int GetLoopDataLength(int bufferLength, int head, int tail) {
            if (head < tail) return tail - head;
            else return bufferLength - head + tail;
        }
)
