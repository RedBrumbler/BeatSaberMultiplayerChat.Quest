#pragma once

#include <stdint.h>
#include <string>
#include <list>

enum class VoiceActivationMode : uint8_t {
    // Push to talk
    Hold = 0,
    // Mic toggle
    Toggle = 1
};

enum class VoiceKeybind : uint8_t {
    // X/A button
    PrimaryButton = 0,
    // Y/B button
    SecondaryButton = 1,
    // Trigger
    Trigger = 2,
    // Thumbstick press
    StickPress = 3,
};

enum class VoiceKeybindController : uint8_t {
    None = 0,
    Left = 1,
    Right = 2,
    Either = Left | Right
};

static inline VoiceKeybindController operator &(VoiceKeybindController first, VoiceKeybindController second) {
    return static_cast<VoiceKeybindController>(static_cast<uint8_t>(first) & static_cast<uint8_t>(second));
}

static inline VoiceKeybindController operator |(VoiceKeybindController first, VoiceKeybindController second) {
    return static_cast<VoiceKeybindController>(static_cast<uint8_t>(first) | static_cast<uint8_t>(second));
}

struct Config {
    bool exportedSoundFilesBefore = false;
#pragma region Text
    bool enableTextchat = true;
    bool enablePlayerBubbles = true;
    bool enableCenterBubbles = true;
    std::string soundNotification = "ClubPing2.ogg";
    float soundNotificationVolume = 0.8f;
#pragma endregion // Text

#pragma region Voice
    bool enableVoiceChat = true;
    // is this needed on quest? there's only 1 mic
    std::string microphoneDevice = "";
    int jitterBufferMs = 50;
    float spatialBlend = 0.1f;
    VoiceActivationMode voiceActivationMode = VoiceActivationMode::Hold;
    VoiceKeybind voiceKeybind = VoiceKeybind::PrimaryButton;
    VoiceKeybindController voiceKeybindController = VoiceKeybindController::Either;
#pragma endregion // Voice

#pragma region HUD
    bool enableHud = true;
    static constexpr const float defaultHudOpacity = .75f;
    static constexpr const float defaultHudOffsetCamX = .3f;
    static constexpr const float defaultHudOffsetCamY = .4f;
    static constexpr const float defaultHudOffsetCamZ = .5f;

    float hudOpacity = defaultHudOpacity;
    float hudOffsetCamX = defaultHudOffsetCamX;
    float hudOffsetCamY = defaultHudOffsetCamY;
    float hudOffsetCamZ = defaultHudOffsetCamZ;
#pragma endregion // HUD

#pragma region Shared
    std::list<std::string> mutedUserIds;
#pragma endregion // Shared
};

extern Config config;
void SaveConfig();
bool LoadConfig();

#include <fmt/core.h>

template <> struct fmt::formatter<::VoiceActivationMode> : formatter<string_view> {
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto format(::VoiceActivationMode c, FormatContext& ctx) {
        switch (c) {
            case VoiceActivationMode::Hold:
                return formatter<string_view>::format("Hold", ctx);
            case VoiceActivationMode::Toggle:
                return formatter<string_view>::format("Toggle", ctx);
            default:
                return formatter<string_view>::format("Unknown", ctx);
        }
    }
};

template <> struct fmt::formatter<::VoiceKeybind> : formatter<string_view> {
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto format(::VoiceKeybind c, FormatContext& ctx) {
        switch (c) {
            case VoiceKeybind::PrimaryButton:
                return formatter<string_view>::format("PrimaryButton", ctx);
            case VoiceKeybind::SecondaryButton:
                return formatter<string_view>::format("SecondaryButton", ctx);
            case VoiceKeybind::Trigger:
                return formatter<string_view>::format("Trigger", ctx);
            case VoiceKeybind::StickPress:
                return formatter<string_view>::format("StickPress", ctx);
            default:
                return formatter<string_view>::format("Unknown", ctx);
        }
    }
};

template <> struct fmt::formatter<::VoiceKeybindController> : formatter<string_view> {
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto format(::VoiceKeybindController c, FormatContext& ctx) {
        switch (c) {
            case VoiceKeybindController::None:
                return formatter<string_view>::format("None", ctx);
            case VoiceKeybindController::Left:
                return formatter<string_view>::format("Left", ctx);
            case VoiceKeybindController::Right:
                return formatter<string_view>::format("Right", ctx);
            case VoiceKeybindController::Either:
                return formatter<string_view>::format("Either", ctx);
            default:
                return formatter<string_view>::format("Unknown", ctx);
        }
    }
};

#include <unordered_map>

static inline VoiceActivationMode parse_VoiceActivationMode(std::string value) {
    static const std::unordered_map<std::string_view, VoiceActivationMode> stringToValue {
        {"Hold", VoiceActivationMode::Hold},
        {"hold", VoiceActivationMode::Hold},
        {"HOLD", VoiceActivationMode::Hold},
        {"Toggle", VoiceActivationMode::Toggle},
        {"toggle", VoiceActivationMode::Toggle},
        {"TOGGLE", VoiceActivationMode::Toggle}
    };

    auto itr = stringToValue.find(value);
    if (itr != stringToValue.end()) return itr->second;

    return VoiceActivationMode::Toggle;
}

static inline VoiceKeybind parse_VoiceKeybind(std::string value) {
    static const std::unordered_map<std::string_view, VoiceKeybind> stringToValue {
        {"PrimaryButton", VoiceKeybind::PrimaryButton},
        {"primarybutton", VoiceKeybind::PrimaryButton},
        {"PRIMARYBUTTON", VoiceKeybind::PrimaryButton},
        {"SecondaryButton", VoiceKeybind::SecondaryButton},
        {"secondarybutton", VoiceKeybind::SecondaryButton},
        {"SECONDARYBUTTON", VoiceKeybind::SecondaryButton},
        {"Trigger", VoiceKeybind::Trigger},
        {"trigger", VoiceKeybind::Trigger},
        {"TRIGGER", VoiceKeybind::Trigger},
        {"StickPress", VoiceKeybind::StickPress},
        {"stickpress", VoiceKeybind::StickPress},
        {"STICKPRESS", VoiceKeybind::StickPress}
    };

    auto itr = stringToValue.find(value);
    if (itr != stringToValue.end()) return itr->second;

    return VoiceKeybind::PrimaryButton;
}

static inline VoiceKeybindController parse_VoiceKeyBindController(std::string value) {
    static const std::unordered_map<std::string_view, VoiceKeybindController> stringToValue {
        {"None", VoiceKeybindController::None},
        {"none", VoiceKeybindController::None},
        {"NONE", VoiceKeybindController::None},
        {"Left", VoiceKeybindController::Left},
        {"left", VoiceKeybindController::Left},
        {"LEFT", VoiceKeybindController::Left},
        {"Right", VoiceKeybindController::Right},
        {"right", VoiceKeybindController::Right},
        {"RIGHT", VoiceKeybindController::Right},
        {"Either", VoiceKeybindController::Either},
        {"either", VoiceKeybindController::Either},
        {"EITHER", VoiceKeybindController::Either}
    };

    auto itr = stringToValue.find(value);
    if (itr != stringToValue.end()) return itr->second;

    return VoiceKeybindController::Either;
}
