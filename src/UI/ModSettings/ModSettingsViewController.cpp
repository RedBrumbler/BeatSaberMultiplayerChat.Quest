#include "UI/ModSettings/ModSettingsViewController.hpp"
#include "UI/MpcColors.hpp"
#include "config.hpp"
#include "assets.hpp"

#include "bsml/shared/BSML.hpp"
#include "UnityEngine/RectTransform.hpp"

DEFINE_TYPE(MultiplayerChat::UI::ModSettings, ModSettingsViewController);

namespace MultiplayerChat::UI::ModSettings {
    void ModSettingsViewController::Inject(Utilities::SpriteManager* spriteManager, Audio::VoiceManager* voiceManager, Audio::MicrophoneManager* microphoneManager, Audio::SoundNotifier* soundNotifier, Core::InputManager* inputManager) {
        _spriteManager = spriteManager;
        _voiceManager = voiceManager;
        _microphoneManager = microphoneManager;
        _soundNotifier = soundNotifier;
        _inputManager = inputManager;
    }

    void ModSettingsViewController::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
        if (firstActivation)
            BSML::parse_and_construct(IncludedAssets::ModSettingsViewController_bsml, get_transform(), this);

        RefreshUIState();

        if (addedToHierarchy) {
            _inputManager->activatedEvent += {&ModSettingsViewController::HandleInputActivate, this};
            _inputManager->deactivatedEvent += {&ModSettingsViewController::HandleInputDeactivate, this};
        }
    }

    void ModSettingsViewController::DidDeactivate(bool removedFromHierarchy, bool screenSystemDisabling) {
        if (removedFromHierarchy) {
            _inputManager->activatedEvent -= {&ModSettingsViewController::HandleInputActivate, this};
            _inputManager->deactivatedEvent -= {&ModSettingsViewController::HandleInputDeactivate, this};
        }
    }

#pragma region Actions
    void ModSettingsViewController::PostParse() {
        _bsmlReady = true;

        _voiceManager->StopLoopbackTest();

        // Make dropdown bigger
        auto trDropdownOuter = reinterpret_cast<UnityEngine::RectTransform*>(dropdownMic->get_transform());
        trDropdownOuter->set_sizeDelta({64, 0});
        auto trDropdownText = reinterpret_cast<UnityEngine::RectTransform*>(trDropdownOuter->Find("DropDownButton/Text"));
        trDropdownText->set_anchorMin({0, .5f});
        trDropdownText->set_anchorMax({1, .5f});

        RefreshUIState();
    }

    void ModSettingsViewController::HandleBtnTestMicClick() {
        if (_voiceManager->get_isLoopbackTesting())
            _voiceManager->StopLoopbackTest();
        else
            _voiceManager->StartLoopbackTest();

        RefreshUIState();
    }

    void ModSettingsViewController::HandleBtnResetHudOffsetClick() {
        set_enableHud(true);

        set_hudOpacity(Config::defaultHudOpacity);
        set_hudOffsetCamX(Config::defaultHudOffsetCamX);
        set_hudOffsetCamY(Config::defaultHudOffsetCamY);
        set_hudOffsetCamZ(Config::defaultHudOffsetCamZ);

        RefreshUIState();
    }

#pragma endregion // Actions

#pragma region UI shared
    void ModSettingsViewController::RefreshUIState() {
        if (!_bsmlReady)
            return;

        // Text
        dropdownNotification->set_interactable(get_enableTextChat());
        togglePlayerBubbles->set_interactable(get_enableTextChat());
        toggleCenterBubbles->set_interactable(get_enableTextChat());

        // Voice
        toggleVoice->set_interactable(!_voiceManager->get_isLoopbackTesting());
        dropdownMic->set_interactable(get_enableVoiceChat() && !_voiceManager->get_isLoopbackTesting());

        bool keybindsInteractable = get_enableVoiceChat() && !_voiceManager->get_isLoopbackTesting() && _microphoneManager->get_haveSelectedDevice();
        dropdownActivation->set_interactable(keybindsInteractable);
        dropdownKeybind->set_interactable(keybindsInteractable);
        dropdownController->set_interactable(keybindsInteractable);

        if (_voiceManager->get_isLoopbackTesting()) {
            btnTestMic->set_interactable(true);
            btnTestMic->GetComponentInChildren<TMPro::TextMeshProUGUI*>()->SetText("<color=#ff3b3b>Testing mic</color>");
            imgTestMic->set_sprite(_spriteManager->get_micOnIcon());
            imgTestMic->set_color(MpcColors::Green);
        } else {
            btnTestMic->set_interactable(get_enableVoiceChat() && _microphoneManager->get_haveSelectedDevice());
            btnTestMic->GetComponentInChildren<TMPro::TextMeshProUGUI*>()->SetText("<color=#ffffff>Test mic</color>");
            imgTestMic->set_sprite(_spriteManager->get_micOffIcon());
            imgTestMic->set_color({0.5, 0.5, 0.5, 1});
        }

        // Activation text
        if (!config.enableVoiceChat)
            activationText->set_text("Voice chat is completely disabled. You won't be able to speak or hear others.");
        else if (!_microphoneManager->get_haveSelectedDevice())
            activationText->set_text("No microphone selected. You can't speak, but will be able to hear others.");
        else if (config.enableVoiceChat)
            activationText->set_text(fmt::format("While the settings are open, you can test your keybind to control the mic test\r\n<color=#3498db>{}</color>", _inputManager->DescribeKeybindConfig()));

        // HUD
        toggleHud->set_interactable(get_enableVoiceChat());

        auto canSetHudOptions = get_enableVoiceChat() && get_enableHud();

        sliderHudOpacity->set_interactable(canSetHudOptions);
        sliderHudOffsetCamX->set_interactable(canSetHudOptions);
        sliderHudOffsetCamY->set_interactable(canSetHudOptions);
        sliderHudOffsetCamZ->set_interactable(canSetHudOptions);
        btnResetHudOffset->set_interactable(canSetHudOptions);
    }
#pragma endregion // UI shared

#pragma region Settings/Bindings
    bool ModSettingsViewController::get_enableTextChat() { return config.enableTextchat; }
    void ModSettingsViewController::set_enableTextChat(bool value) {
        config.enableTextchat = value;
        SaveConfig();
        RefreshUIState();
    }

    StringW ModSettingsViewController::get_soundNotification() { return !config.soundNotification.empty() ? config.soundNotification : "None"; }
    void ModSettingsViewController::set_soundNotification(StringW value) {
        if (value != "None" && get_enableTextChat())
            _soundNotifier->LoadAndPlayPreview(value);
        config.soundNotification = std::string(value);
        SaveConfig();
        RefreshUIState();
    }

    bool ModSettingsViewController::get_enablePlayerBubbles() { return config.enablePlayerBubbles; }
    void ModSettingsViewController::set_enablePlayerBubbles(bool value) {
        config.enablePlayerBubbles = value;
        SaveConfig();
        RefreshUIState();
    }

    bool ModSettingsViewController::get_enableCenterBubbles() { return config.enableCenterBubbles; }
    void ModSettingsViewController::set_enableCenterBubbles(bool value) {
        config.enableCenterBubbles = value;
        SaveConfig();
        RefreshUIState();
    }

    bool ModSettingsViewController::get_enableVoiceChat() { return config.enableVoiceChat; }
    void ModSettingsViewController::set_enableVoiceChat(bool value) {
        config.enableVoiceChat = value;
        if (!value)
            _voiceManager->StopLoopbackTest();
        SaveConfig();
        RefreshUIState();
    }

    StringW ModSettingsViewController::get_recordingDevice() {
        auto selectedDevice = _microphoneManager->get_selectedDeviceName();
        if (config.microphoneDevice == "None" && (!selectedDevice || selectedDevice == "None")) return "None";
        selectedDevice = selectedDevice ? selectedDevice : "Default";

        return get_microphoneOptions()->Contains(selectedDevice) ? selectedDevice : "None";
    }

    void ModSettingsViewController::set_recordingDevice(StringW value) {
        _microphoneManager->TrySelectDevice(value);
        config.microphoneDevice = std::string(value);
        SaveConfig();
        RefreshUIState();
    }

    StringW ModSettingsViewController::get_voiceActivationMode() { return fmt::format("{}", config.voiceActivationMode); }
    void ModSettingsViewController::set_voiceActivationMode(StringW value) {
        config.voiceActivationMode = parse_VoiceActivationMode(value);
        SaveConfig();
        RefreshUIState();
    }

    StringW ModSettingsViewController::get_voiceKeyBind() { return fmt::format("{}", config.voiceKeybind); }
    void ModSettingsViewController::set_voiceKeyBind(StringW value) {
        config.voiceKeybind = parse_VoiceKeybind(value);
        SaveConfig();
        RefreshUIState();
    }

    StringW ModSettingsViewController::get_voiceKeyBindController() { return fmt::format("{}", config.voiceKeybindController); }
    void ModSettingsViewController::set_voiceKeyBindController(StringW value) {
        config.voiceKeybindController = parse_VoiceKeyBindController(value);
        SaveConfig();
        RefreshUIState();
    }

    bool ModSettingsViewController::get_enableHud() { return config.enableHud; }
    void ModSettingsViewController::set_enableHud(bool value) {
        config.enableHud = value;
        SaveConfig();
        RefreshUIState();
    }

    float ModSettingsViewController::get_hudOpacity() { return config.hudOpacity; }
    void ModSettingsViewController::set_hudOpacity(float value) {
        // TODO: update hud
        config.hudOpacity = value;
        SaveConfig();
        RefreshUIState();
    }

    float ModSettingsViewController::get_hudOffsetCamX() { return config.hudOffsetCamX; }
    void ModSettingsViewController::set_hudOffsetCamX(float value) {
        // TODO: update hud
        config.hudOffsetCamX = value;
        SaveConfig();
        RefreshUIState();
    }

    float ModSettingsViewController::get_hudOffsetCamY() { return config.hudOffsetCamY; }
    void ModSettingsViewController::set_hudOffsetCamY(float value) {
        // TODO: update hud
        config.hudOffsetCamY = value;
        SaveConfig();
        RefreshUIState();
    }

    float ModSettingsViewController::get_hudOffsetCamZ() { return config.hudOffsetCamZ; }
    void ModSettingsViewController::set_hudOffsetCamZ(float value) {
        // TODO: update hud
        config.hudOffsetCamZ = value;
        SaveConfig();
        RefreshUIState();
    }

#pragma endregion // Settings/Bindings

#pragma region Option lists
    ListWrapper<Il2CppObject*> ModSettingsViewController::get_soundNotificationOptions() {
        auto availableSounds = _soundNotifier->GetAvailableClipNames();

        ListWrapper<Il2CppObject*> list = List<Il2CppObject*>::New_ctor();
        list->EnsureCapacity(availableSounds.size() + 1);
        list->Add(StringW("None"));
        for (const auto& sound : availableSounds) list->Add(StringW(sound));

        return list;
    }

    ListWrapper<Il2CppObject*> ModSettingsViewController::get_microphoneOptions() {
        auto availableDevices = _microphoneManager->GetAvailableDeviceNames();

        ListWrapper<Il2CppObject*> list = List<Il2CppObject*>::New_ctor();
        list->EnsureCapacity(availableDevices.size() + 1);
        list->Add(StringW("None"));
        list->Add(StringW("Default"));
        for (auto sound : availableDevices) list->Add(sound);

        return list;
    }

    ListWrapper<Il2CppObject*> ModSettingsViewController::get_activationOptions() {
        ListWrapper<Il2CppObject*> list = List<Il2CppObject*>::New_ctor();
        list->EnsureCapacity(2);
        list->Add(StringW("Hold"));
        list->Add(StringW("Toggle"));
        return list;
    }

    ListWrapper<Il2CppObject*> ModSettingsViewController::get_keybindOptions() {
        ListWrapper<Il2CppObject*> list = List<Il2CppObject*>::New_ctor();
        list->EnsureCapacity(4);
        list->Add(StringW("PrimaryButton"));
        list->Add(StringW("SecondaryButton"));
        list->Add(StringW("Trigger"));
        list->Add(StringW("StickPress"));
        return list;
    }

    ListWrapper<Il2CppObject*> ModSettingsViewController::get_controllerOptions() {
        ListWrapper<Il2CppObject*> list = List<Il2CppObject*>::New_ctor();
        list->EnsureCapacity(3);
        list->Add(StringW("Left"));
        list->Add(StringW("Right"));
        list->Add(StringW("Either"));
        return list;
    }

#pragma endregion // Option lists

#pragma region Input events
    void ModSettingsViewController::HandleInputActivate() { RefreshUIState(); }

    void ModSettingsViewController::HandleInputDeactivate() { RefreshUIState(); }

#pragma endregion // Input events
}
