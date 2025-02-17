#pragma once

#include "custom-types/shared/macros.hpp"
#include "lapiz/shared/macros.hpp"
#include "UnityEngine/MonoBehaviour.hpp"

#include "Zenject/IInitializable.hpp"
#include "System/IDisposable.hpp"

#include "Audio/VoiceManager.hpp"
#include "GlobalNamespace/HapticFeedbackManager.hpp"
#include "Audio/SoundNotifier.hpp"

#include "UnityEngine/XR/XRNode.hpp"
#include "UnityEngine/XR/InputDevice.hpp"
#include "UnityEngine/XR/InputFeatureUsage.hpp"

#include "Libraries/HM/HMLib/VR/HapticPresetSO.hpp"

DECLARE_CLASS_CODEGEN_INTERFACES(MultiplayerChat::Core, InputManager, UnityEngine::MonoBehaviour, std::vector<Il2CppClass*>({ classof(Zenject::IInitializable*), classof(System::IDisposable*)}),
    DECLARE_INSTANCE_FIELD_PRIVATE(Audio::VoiceManager*, _voiceManager);
    DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::HapticFeedbackManager*, _hapticFeedback);
    DECLARE_INSTANCE_FIELD_PRIVATE(Audio::SoundNotifier*, _soundNotifier);

    DECLARE_INSTANCE_FIELD_PRIVATE(System::Action_1<UnityEngine::XR::InputDevice>*, _deviceConnectedAction);
    DECLARE_INSTANCE_FIELD_PRIVATE(System::Action_1<UnityEngine::XR::InputDevice>*, _deviceDisconnectedAction);

    DECLARE_INSTANCE_FIELD(bool, _isSongPlaying);

    DECLARE_INSTANCE_FIELD_PRIVATE(Libraries::HM::HMLib::VR::HapticPresetSO*, _hapticPulsePreset);

    DECLARE_OVERRIDE_METHOD_MATCH(void, Initialize, &::Zenject::IInitializable::Initialize);
    DECLARE_OVERRIDE_METHOD_MATCH(void, Dispose, &::System::IDisposable::Dispose);

    DECLARE_INSTANCE_METHOD(void, Awake);
    DECLARE_INSTANCE_METHOD(void, OnEnable);
    DECLARE_INSTANCE_METHOD(void, OnDisable);
    DECLARE_INSTANCE_METHOD(void, Update);

    DECLARE_INJECT_METHOD(void, Inject, Audio::VoiceManager* voiceManager, GlobalNamespace::HapticFeedbackManager* hapticFeedback, Audio::SoundNotifier* soundNotifier);

    // DECLARE_CTOR(ctor);
    DECLARE_DEFAULT_CTOR();
    public:
        UnorderedEventCallback<> activatedEvent;
        UnorderedEventCallback<> deactivatedEvent;
        UnorderedEventCallback<> testModeChangedEvent;

        void SetActive(bool active);

        bool get_testMode() const { return _testMode; }
        void set_testMode(bool value) {
            _testMode = value;
            testModeChangedEvent.invoke();
        }

        std::string DescribeKeybindConfig();
    private:
        static UnityEngine::XR::InputDevice _leftController;
        static UnityEngine::XR::InputDevice _rightController;

        bool _triggerConditionIsActive;
        bool _debugKeyIsDown;
        bool _testMode;

        bool get_triggerIsActive() const { return _voiceManager->get_isTransmitting() || _voiceManager->get_isLoopbackTesting(); }
        void TriggerActivate();
        void TriggerDeactivate();

        void HandleKeybindDown();
        void HandleKeybindUp();
        void PlayActivationEffect();
        void PlayDeactivationEffect();

        void HandleInputDeviceConnected(UnityEngine::XR::InputDevice device);
        void HandleInputDeviceDisconnected(UnityEngine::XR::InputDevice device);

        bool CheckTriggerCondition();
        bool CheckTriggerConditionOnDevice(UnityEngine::XR::InputDevice device);
        static bool GetInputButtonIsDown(UnityEngine::XR::InputDevice device, UnityEngine::XR::InputFeatureUsage usage);
        static bool GetInputValueThreshold(UnityEngine::XR::InputDevice device, UnityEngine::XR::InputFeatureUsage usage, float threshold);

        void PreloadSounds();
        void PlayMicOnSound();
        void PlayMicOffSound();

        static Libraries::HM::HMLib::VR::HapticPresetSO* CreateHapticPreset();
        void HapticPulse(UnityEngine::XR::XRNode node);
)
