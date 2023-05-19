#include "Core/InputManager.hpp"
#include "Utilities/InputDevicesExtension.hpp"

#include "custom-types/shared/delegate.hpp"
#include "config.hpp"
#include "logging.hpp"

#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/XR/InputDevice.hpp"
#include "UnityEngine/XR/InputDevices.hpp"
#include "UnityEngine/XR/InputTracking.hpp"

DEFINE_TYPE(MultiplayerChat::Core, InputManager);

namespace MultiplayerChat::Core {
    UnityEngine::XR::InputDevice InputManager::_leftController;
    UnityEngine::XR::InputDevice InputManager::_rightController;
    void InputManager::ctor() {
        INVOKE_CTOR();
        _hapticPulsePreset = CreateHapticPreset();
    }

    void InputManager::Inject(Audio::VoiceManager* voiceManager, GlobalNamespace::HapticFeedbackController* hapticFeedback, Audio::SoundNotifier* soundNotifier) {
        _voiceManager = voiceManager;
        _hapticFeedback = hapticFeedback;
        _soundNotifier = soundNotifier;
    }

    void InputManager::Initialize() {
        _leftController = Utilities::InputDevicesExtension::GetDeviceAtXRNode(UnityEngine::XR::XRNode::LeftHand);
        _rightController = Utilities::InputDevicesExtension::GetDeviceAtXRNode(UnityEngine::XR::XRNode::RightHand);

        _deviceConnectedAction = custom_types::MakeDelegate<decltype(_deviceConnectedAction)>(std::function<void(UnityEngine::XR::InputDevice)>(
            std::bind(&InputManager::HandleInputDeviceConnected, this, std::placeholders::_1)
        ));
        _deviceDisconnectedAction = custom_types::MakeDelegate<decltype(_deviceDisconnectedAction)>(std::function<void(UnityEngine::XR::InputDevice)>(
            std::bind(&InputManager::HandleInputDeviceDisconnected, this, std::placeholders::_1)
        ));

        auto combinedConnectedAction = reinterpret_cast<System::Action_1<UnityEngine::XR::InputDevice>*>(System::Delegate::Combine(UnityEngine::XR::InputDevices::_get_deviceConnected(), _deviceConnectedAction));
        UnityEngine::XR::InputDevices::_set_deviceConnected(combinedConnectedAction);

        auto combinedDisconnectedAction = reinterpret_cast<System::Action_1<UnityEngine::XR::InputDevice>*>(System::Delegate::Combine(UnityEngine::XR::InputDevices::_get_deviceDisconnected(), _deviceDisconnectedAction));
        UnityEngine::XR::InputDevices::_set_deviceDisconnected(combinedDisconnectedAction);

        PreloadSounds();

        _triggerConditionIsActive = false;
        _debugKeyIsDown = false;

        set_testMode(false);
        SetActive(false);
    }

    void InputManager::Dispose() {
        auto unCombinedConnectedAction = reinterpret_cast<System::Action_1<UnityEngine::XR::InputDevice>*>(System::Delegate::Remove(UnityEngine::XR::InputDevices::_get_deviceConnected(), _deviceConnectedAction));
        UnityEngine::XR::InputDevices::_set_deviceConnected(unCombinedConnectedAction);

        auto unCombinedDisconnectedAction = reinterpret_cast<System::Action_1<UnityEngine::XR::InputDevice>*>(System::Delegate::Remove(UnityEngine::XR::InputDevices::_get_deviceDisconnected(), _deviceDisconnectedAction));
        UnityEngine::XR::InputDevices::_set_deviceDisconnected(unCombinedDisconnectedAction);

        _triggerConditionIsActive = false;
        _debugKeyIsDown = false;

        set_testMode(false);

        if (_hapticPulsePreset && _hapticPulsePreset->m_CachedPtr.m_value)
            UnityEngine::Object::Destroy(_hapticPulsePreset);
        _hapticPulsePreset = nullptr;
    }

    void InputManager::OnEnable() {
        _triggerConditionIsActive = false;
        _debugKeyIsDown = false;
    }

    void InputManager::OnDisable() {
        if (get_triggerIsActive())
            TriggerDeactivate();
        _debugKeyIsDown = false;
    }

    void InputManager::Update() {
        auto wasActive = _triggerConditionIsActive;
        auto isActive = CheckTriggerCondition();

        if (wasActive && !isActive) { // trigger release
            _triggerConditionIsActive = false;
            HandleKeybindUp();
        } else if (!wasActive && isActive) { // trigger down
            _triggerConditionIsActive = true;
            HandleKeybindDown();
        }
    }

#pragma region Activation
    void InputManager::SetActive(bool active) {
        get_gameObject()->SetActive(active);
    }

    void InputManager::TriggerActivate() {
        if (!config.enableVoiceChat) return;
        if (config.microphoneDevice == "None") return;
        if (get_testMode())
            _voiceManager->StartLoopbackTest();
        else if (!_voiceManager->StartVoiceTransmission()) return;

        PlayActivationEffect();

        activatedEvent.invoke();
    }

    void InputManager::TriggerDeactivate() {
        if (!get_triggerIsActive()) return;
        _voiceManager->StopLoopbackTest();
        _voiceManager->StopVoiceTransmission();

        PlayDeactivationEffect();

        deactivatedEvent.invoke();
    }

#pragma endregion // Activation

#pragma region Keybind activation
    void InputManager::HandleKeybindDown() {
        switch(config.voiceActivationMode) {
            case VoiceActivationMode::Toggle:
                if (get_triggerIsActive())
                    TriggerDeactivate();
                else
                    TriggerActivate();
                break;
            case VoiceActivationMode::Hold:
                if (!get_triggerIsActive())
                    TriggerActivate();
                break;
        }
    }

    void InputManager::HandleKeybindUp() {
        switch (config.voiceActivationMode) {
            case VoiceActivationMode::Hold:
                if (get_triggerIsActive())
                    TriggerDeactivate();
                break;
            default:
                break;
        }
    }

    void InputManager::PlayActivationEffect() {
        PlayMicOnSound();

        if ((config.voiceKeybindController & VoiceKeybindController::Left) == VoiceKeybindController::Left)
            HapticPulse(UnityEngine::XR::XRNode::LeftHand);
        if ((config.voiceKeybindController & VoiceKeybindController::Right) == VoiceKeybindController::Right)
            HapticPulse(UnityEngine::XR::XRNode::RightHand);
    }

    void InputManager::PlayDeactivationEffect() {
        PlayMicOffSound();

        if ((config.voiceKeybindController & VoiceKeybindController::Left) == VoiceKeybindController::Left)
            HapticPulse(UnityEngine::XR::XRNode::LeftHand);
        if ((config.voiceKeybindController & VoiceKeybindController::Right) == VoiceKeybindController::Right)
            HapticPulse(UnityEngine::XR::XRNode::RightHand);
    }

#pragma endregion // Keybind activation

#pragma region Device management
    void InputManager::HandleInputDeviceConnected(UnityEngine::XR::InputDevice device) {
        if (!Utilities::InputDevicesExtension::IsDeviceValid(device)) return;

        DEBUG("Input device connected: {}", Utilities::InputDevicesExtension::GetDeviceName(device));

        auto chars = Utilities::InputDevicesExtension::GetDeviceCharacteristics(device);
        if (chars == Utilities::InputDeviceCharacteristics::None ||
            (chars & Utilities::InputDeviceCharacteristics::HeldInHand) == Utilities::InputDeviceCharacteristics::None ||
            (chars & Utilities::InputDeviceCharacteristics::Controller) == Utilities::InputDeviceCharacteristics::None)
            // not handheld
            return;

        if ((chars & Utilities::InputDeviceCharacteristics::Left) != Utilities::InputDeviceCharacteristics::None)
            _leftController = device;

        if ((chars & Utilities::InputDeviceCharacteristics::Right) != Utilities::InputDeviceCharacteristics::None)
            _rightController = device;
    }

    void InputManager::HandleInputDeviceDisconnected(UnityEngine::XR::InputDevice device) {
        DEBUG("Input device disconnected: {}", Utilities::InputDevicesExtension::GetDeviceName(device));

        if (device.Equals(_leftController))
            _leftController = Utilities::InputDevicesExtension::GetDeviceAtXRNode(UnityEngine::XR::XRNode::LeftHand);
        if (device.Equals(_rightController))
            _rightController = Utilities::InputDevicesExtension::GetDeviceAtXRNode(UnityEngine::XR::XRNode::LeftHand);
    }

#pragma endregion // Device management

#pragma region Device states
    bool InputManager::CheckTriggerCondition() {
        switch(config.voiceKeybindController) {
            case VoiceKeybindController::Either:
                return CheckTriggerConditionOnDevice(_leftController) || CheckTriggerConditionOnDevice(_rightController);
            case VoiceKeybindController::Left:
                return CheckTriggerConditionOnDevice(_leftController);
            case VoiceKeybindController::Right:
                return CheckTriggerConditionOnDevice(_rightController);
            default:
                return false;
        }
    }

    // wraps what otherwise would be available in unity
    namespace CommonUsages {
        inline UnityEngine::XR::InputFeatureUsage get_primaryButton() {
            static ConstString str("PrimaryButton");
            static auto usage = UnityEngine::XR::InputFeatureUsage(str, UnityEngine::XR::InputFeatureType::Binary);
            return usage;
        }

        inline UnityEngine::XR::InputFeatureUsage get_secondaryButton() {
            static ConstString str("SecondaryButton");
            static auto usage = UnityEngine::XR::InputFeatureUsage(str, UnityEngine::XR::InputFeatureType::Binary);
            return usage;
        }

        inline UnityEngine::XR::InputFeatureUsage get_trigger() {
            static ConstString str("Trigger");
            static auto usage = UnityEngine::XR::InputFeatureUsage(str, UnityEngine::XR::InputFeatureType::Axis1D);
            return usage;
        }

        inline UnityEngine::XR::InputFeatureUsage get_primary2DAxisClick() {
            static ConstString str("Primary2DAxisClick");
            static auto usage = UnityEngine::XR::InputFeatureUsage(str, UnityEngine::XR::InputFeatureType::Binary);
            return usage;
        }
    };

    bool InputManager::CheckTriggerConditionOnDevice(UnityEngine::XR::InputDevice device) {
        switch (config.voiceKeybind) {
            case VoiceKeybind::PrimaryButton:
                return GetInputButtonIsDown(device, CommonUsages::get_primaryButton());
            case VoiceKeybind::SecondaryButton:
                return GetInputButtonIsDown(device, CommonUsages::get_secondaryButton());
            case VoiceKeybind::Trigger:
                return GetInputValueThreshold(device, CommonUsages::get_trigger(), .85f);
            case VoiceKeybind::StickPress:
                return GetInputButtonIsDown(device, CommonUsages::get_primary2DAxisClick());
            default:
                return false;
        }
    }

    bool InputManager::GetInputButtonIsDown(UnityEngine::XR::InputDevice device, UnityEngine::XR::InputFeatureUsage usage) {
        if (!Utilities::InputDevicesExtension::IsDeviceValid(device)) return false;

        bool result;
        return Utilities::InputDevicesExtension::TryGetFeatureValue(device, usage, result) && result;
    }

    bool InputManager::GetInputValueThreshold(UnityEngine::XR::InputDevice device, UnityEngine::XR::InputFeatureUsage usage, float threshold) {
        if (!Utilities::InputDevicesExtension::IsDeviceValid(device)) return false;

        float result;
        return Utilities::InputDevicesExtension::TryGetFeatureValue(device, usage, result) && result > threshold;
    }

#pragma endregion // Device states

#pragma region Sound effects
    void InputManager::PreloadSounds() {
        _soundNotifier->LoadClipIfNeeded("MicOn");
        _soundNotifier->LoadClipIfNeeded("MicOff");
    }

    void InputManager::PlayMicOnSound() {
        _soundNotifier->LoadAndPlayPreview("MicOn");
    }

    void InputManager::PlayMicOffSound() {
        _soundNotifier->LoadAndPlayPreview("MicOff");
    }

#pragma endregion // Sound effects

#pragma region Haptics
    Libraries::HM::HMLib::VR::HapticPresetSO* InputManager::CreateHapticPreset() {
        auto hapticPulsePreset = UnityEngine::ScriptableObject::CreateInstance<Libraries::HM::HMLib::VR::HapticPresetSO*>();
        hapticPulsePreset->continuous = false;
        hapticPulsePreset->duration = 0.1f;
        hapticPulsePreset->frequency = 0.25f;
        hapticPulsePreset->strength = 0.5f;
        return hapticPulsePreset;
    }

    void InputManager::HapticPulse(UnityEngine::XR::XRNode node) {
        _hapticFeedback->PlayHapticFeedback(node, _hapticPulsePreset);
    }

#pragma endregion // Haptics

#pragma region Settings
    std::string InputManager::DescribeKeybindConfig() {
        if (!config.enableVoiceChat) return "Voice chat is disabled";

        std::string keybindDescr;
        switch (config.voiceKeybind) {
            case VoiceKeybind::PrimaryButton:
                keybindDescr = "the primary button";
                break;
            case VoiceKeybind::SecondaryButton:
                keybindDescr = "the secondary button";
                break;
            case VoiceKeybind::Trigger:
                keybindDescr = "the trigger";
                break;
            case VoiceKeybind::StickPress:
                keybindDescr = "the joystick down";
                break;
            default:
                keybindDescr = "(Unknown button)";
                break;
        }

        std::string controllerText;
        switch (config.voiceKeybindController) {
            default: [[fallthrough]];
            case VoiceKeybindController::Either:
                controllerText = "on either controller";
                break;
            case VoiceKeybindController::Left:
                controllerText = "on the left controller";
                break;
            case VoiceKeybindController::Right:
                controllerText = "on the right controller";
                break;
            case VoiceKeybindController::None:
                controllerText = "on no controller";
                break;
        }

        switch (config.voiceActivationMode) {
            case VoiceActivationMode::Hold:
                return fmt::format("Hold {} {} to speak", keybindDescr, controllerText);
            default: [[fallthrough]];
            case VoiceActivationMode::Toggle:
                return fmt::format("Press {} {} to unmute/mute", keybindDescr, controllerText);
        }
    }

#pragma endregion // Settings
}
