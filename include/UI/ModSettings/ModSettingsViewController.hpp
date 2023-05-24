#pragma once

#include "custom-types/shared/macros.hpp"
#include "lapiz/shared/macros.hpp"
#include "bsml/shared/macros.hpp"

#include "HMUI/ViewController.hpp"
#include "UnityEngine/GameObject.hpp"

#include "Utilities/SpriteManager.hpp"
#include "Audio/VoiceManager.hpp"
#include "Audio/MicrophoneManager.hpp"
#include "Audio/SoundNotifier.hpp"
#include "Core/InputManager.hpp"

#include "bsml/shared/BSML/Components/Settings/DropdownListSetting.hpp"
#include "bsml/shared/BSML/Components/Settings/ToggleSetting.hpp"
#include "bsml/shared/BSML/Components/Settings/SliderSetting.hpp"
#include "HMUI/CurvedTextMeshPro.hpp"
#include "HMUI/ImageView.hpp"
#include "UnityEngine/UI/Button.hpp"

DECLARE_CLASS_CODEGEN(MultiplayerChat::UI::ModSettings, ModSettingsViewController, HMUI::ViewController,
    DECLARE_INSTANCE_FIELD_PRIVATE(Utilities::SpriteManager*, _spriteManager);
    DECLARE_INSTANCE_FIELD_PRIVATE(Audio::VoiceManager*, _voiceManager);
    DECLARE_INSTANCE_FIELD_PRIVATE(Audio::MicrophoneManager*, _microphoneManager);
    DECLARE_INSTANCE_FIELD_PRIVATE(Audio::SoundNotifier*, _soundNotifier);
    DECLARE_INSTANCE_FIELD_PRIVATE(Core::InputManager*, _inputManager);

    DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::GameObject*, noVoiceSettingsNotice);
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::GameObject*, voiceSettings);
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::UI::Button*, btnTestMic);
    DECLARE_INSTANCE_FIELD_PRIVATE(BSML::DropdownListSetting*, dropdownNotification);
    DECLARE_INSTANCE_FIELD_PRIVATE(BSML::ToggleSetting*, toggleVoice);
    DECLARE_INSTANCE_FIELD_PRIVATE(BSML::DropdownListSetting*, dropdownMic);
    DECLARE_INSTANCE_FIELD_PRIVATE(BSML::SliderSetting*, sliderMicGain);
    DECLARE_INSTANCE_FIELD_PRIVATE(HMUI::ImageView*, imgTestMic);
    DECLARE_INSTANCE_FIELD_PRIVATE(BSML::DropdownListSetting*, dropdownActivation);
    DECLARE_INSTANCE_FIELD_PRIVATE(BSML::DropdownListSetting*, dropdownKeybind);
    DECLARE_INSTANCE_FIELD_PRIVATE(BSML::DropdownListSetting*, dropdownController);
    DECLARE_INSTANCE_FIELD_PRIVATE(BSML::DropdownListSetting*, dropdownControllerAlt);
    DECLARE_INSTANCE_FIELD_PRIVATE(HMUI::CurvedTextMeshPro*, activationText);
    DECLARE_INSTANCE_FIELD_PRIVATE(BSML::ToggleSetting*, togglePlayerBubbles);
    DECLARE_INSTANCE_FIELD_PRIVATE(BSML::ToggleSetting*, toggleCenterBubbles);
    DECLARE_INSTANCE_FIELD_PRIVATE(BSML::ToggleSetting*, toggleHud);
    DECLARE_INSTANCE_FIELD_PRIVATE(BSML::SliderSetting*, sliderHudOpacity);
    DECLARE_INSTANCE_FIELD_PRIVATE(BSML::SliderSetting*, sliderHudOffsetCamX);
    DECLARE_INSTANCE_FIELD_PRIVATE(BSML::SliderSetting*, sliderHudOffsetCamY);
    DECLARE_INSTANCE_FIELD_PRIVATE(BSML::SliderSetting*, sliderHudOffsetCamZ);
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::UI::Button*, btnResetHudOffset);
    DECLARE_INSTANCE_FIELD_PRIVATE(bool, _bsmlReady);

    DECLARE_INSTANCE_METHOD(void, PostParse);
    DECLARE_INSTANCE_METHOD(void, HandleRecheckMicrophonesClick);
    DECLARE_INSTANCE_METHOD(void, HandleBtnTestMicClick);
    DECLARE_INSTANCE_METHOD(void, HandleBtnResetHudOffsetClick);

    DECLARE_BSML_PROPERTY(bool, enableTextChat);
    DECLARE_BSML_PROPERTY(StringW, soundNotification);
    DECLARE_BSML_PROPERTY(bool, enablePlayerBubbles);
    DECLARE_BSML_PROPERTY(bool, enableCenterBubbles);
    DECLARE_BSML_PROPERTY(bool, enableVoiceChat);
    DECLARE_BSML_PROPERTY(StringW, recordingDevice);
    DECLARE_BSML_PROPERTY(float, micGain);
    DECLARE_BSML_PROPERTY(StringW, voiceActivationMode);
    DECLARE_BSML_PROPERTY(StringW, voiceKeyBind);
    DECLARE_BSML_PROPERTY(StringW, voiceKeyBindController);
    DECLARE_BSML_PROPERTY(bool, enableHud);
    DECLARE_BSML_PROPERTY(float, hudOpacity);
    DECLARE_BSML_PROPERTY(float, hudOffsetCamX);
    DECLARE_BSML_PROPERTY(float, hudOffsetCamY);
    DECLARE_BSML_PROPERTY(float, hudOffsetCamZ);
    DECLARE_INSTANCE_METHOD(bool, get_hasNoPermission);
    DECLARE_INSTANCE_METHOD(bool, get_hasPermission);

    DECLARE_INSTANCE_METHOD(ListWrapper<Il2CppObject*>, get_soundNotificationOptions);
    DECLARE_INSTANCE_METHOD(ListWrapper<Il2CppObject*>, get_microphoneOptions);
    DECLARE_INSTANCE_METHOD(ListWrapper<Il2CppObject*>, get_activationOptions);
    DECLARE_INSTANCE_METHOD(ListWrapper<Il2CppObject*>, get_keybindOptions);
    DECLARE_INSTANCE_METHOD(ListWrapper<Il2CppObject*>, get_controllerOptions);
    DECLARE_INSTANCE_METHOD(ListWrapper<Il2CppObject*>, get_controllerOptionsAlt);

    DECLARE_OVERRIDE_METHOD(void, DidActivate, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::HMUI::ViewController::DidActivate>::get(), bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);
    DECLARE_OVERRIDE_METHOD(void, DidDeactivate, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::HMUI::ViewController::DidDeactivate>::get(), bool removedFromHierarchy, bool screenSystemDisabling);

    DECLARE_INJECT_METHOD(void, Inject, Utilities::SpriteManager* spriteManager, Audio::VoiceManager* voiceManager, Audio::MicrophoneManager* microphoneManager, Audio::SoundNotifier* soundNotifier, Core::InputManager* inputManager);
    public:
        void RefreshUIState();
        void HandleInputActivate();
        void HandleInputDeactivate();
)
