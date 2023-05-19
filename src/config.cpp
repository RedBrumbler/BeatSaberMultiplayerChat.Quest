#include "config.hpp"
#include "logging.hpp"
#include "beatsaber-hook/shared/config/config-utils.hpp"

Config config;

extern ModInfo modInfo;

Configuration& get_config() {
    static Configuration cfg(modInfo);
    cfg.Load();
    return cfg;
}
#define ADD(member) add(cfg, #member, config.member, allocator)

static inline void add(rapidjson::Document& doc, rapidjson::Document::StringRefType identifier, bool value, rapidjson::Document::AllocatorType& allocator) {
    doc.AddMember(identifier, value, allocator);
}

static inline void add(rapidjson::Document& doc, rapidjson::Document::StringRefType identifier, float value, rapidjson::Document::AllocatorType& allocator) {
    doc.AddMember(identifier, value, allocator);
}

static inline void add(rapidjson::Document& doc, rapidjson::Document::StringRefType identifier, int value, rapidjson::Document::AllocatorType& allocator) {
    doc.AddMember(identifier, value, allocator);
}

static inline void add(rapidjson::Document& doc, rapidjson::Document::StringRefType identifier, std::string& value, rapidjson::Document::AllocatorType& allocator) {
    doc.AddMember(identifier, rapidjson::Value(value.c_str(), value.size(), allocator), allocator);
}

template<typename T>
requires(std::is_enum_v<T>)
static inline void add(rapidjson::Document& doc, rapidjson::Document::StringRefType identifier, T value, rapidjson::Document::AllocatorType& allocator) {
    doc.AddMember(identifier, static_cast<int>(value), allocator);
}

static void add(rapidjson::Document& doc, rapidjson::Document::StringRefType identifier, std::vector<std::string>& value, rapidjson::Document::AllocatorType& allocator) {
    rapidjson::Value arr;
    arr.SetArray();
    arr.Reserve(value.size(), allocator);
    for (const auto& id : value)
        arr.PushBack(rapidjson::Value(id.c_str(), id.size(), allocator), allocator);
    doc.AddMember(identifier, arr, allocator);
}

static void add(rapidjson::Document& doc, rapidjson::Document::StringRefType identifier, std::list<std::string>& value, rapidjson::Document::AllocatorType& allocator) {
    rapidjson::Value arr;
    arr.SetArray();
    arr.Reserve(value.size(), allocator);
    for (const auto& id : value)
        arr.PushBack(rapidjson::Value(id.c_str(), id.size(), allocator), allocator);
    doc.AddMember(identifier, arr, allocator);
}

void SaveConfig() {
    INFO("Saving Config...");
    auto& cfg = get_config().config;
    auto allocator = cfg.GetAllocator();
    cfg.RemoveAllMembers();
    cfg.SetObject();
    // Text
    ADD(enableTextchat);
    ADD(enablePlayerBubbles);
    ADD(enableCenterBubbles);
    ADD(soundNotification);
    ADD(soundNotificationVolume);

    // Voice
    ADD(enableVoiceChat);
    ADD(microphoneDevice);
    ADD(jitterBufferMs);
    ADD(spatialBlend);
    ADD(voiceActivationMode);
    ADD(voiceKeybind);
    ADD(voiceKeybindController);

    // HUD
    ADD(enableHud);
    ADD(hudOpacity);
    ADD(hudOffsetCamX);
    ADD(hudOffsetCamY);
    ADD(hudOffsetCamZ);

    // Shared
    ADD(mutedUserIds);

    get_config().Write();
    INFO("Config Saved!");
}

#define GET(identifier) if (!get(cfg, #identifier, config.identifier)) foundEverything = false

static bool get(const rapidjson::Document& doc, const std::string& identifier, bool& out) {
    auto itr = doc.FindMember(identifier);
    if (itr != doc.MemberEnd() && itr->value.IsBool()) {
        out = itr->value.GetBool();
        return true;
    }
    return false;
}

static bool get(const rapidjson::Document& doc, const std::string& identifier, float& out) {
    auto itr = doc.FindMember(identifier);
    if (itr != doc.MemberEnd() && itr->value.IsFloat()) {
        out = itr->value.GetFloat();
        return true;
    }
    return false;
}

static bool get(const rapidjson::Document& doc, const std::string& identifier, int& out) {
    auto itr = doc.FindMember(identifier);
    if (itr != doc.MemberEnd() && itr->value.IsInt()) {
        out = itr->value.GetInt();
        return true;
    }
    return false;
}

static bool get(const rapidjson::Document& doc, const std::string& identifier, std::string& out) {
    auto itr = doc.FindMember(identifier);
    if (itr != doc.MemberEnd() && itr->value.IsString()) {
        out = {itr->value.GetString(), itr->value.GetStringLength()};
        return true;
    }
    return false;
}

template<typename T>
requires(std::is_enum_v<T>)
static bool get(const rapidjson::Document& doc, const std::string& identifier, T& out) {
    auto itr = doc.FindMember(identifier);
    if (itr != doc.MemberEnd() && itr->value.IsInt()) {
        out = static_cast<T>(itr->value.GetInt());
        return true;
    }
    return false;
}

static bool get(const rapidjson::Document& doc, const std::string& identifier, std::vector<std::string>& out) {
    auto itr = doc.FindMember(identifier);
    if (itr != doc.MemberEnd() && itr->value.IsArray()) {
        out.clear();
        for (const auto& muted : itr->value.GetArray())
            out.emplace_back(muted.GetString(), muted.GetStringLength());
        return true;
    }
    return false;
}

static bool get(const rapidjson::Document& doc, const std::string& identifier, std::list<std::string>& out) {
    auto itr = doc.FindMember(identifier);
    if (itr != doc.MemberEnd() && itr->value.IsArray()) {
        out.clear();
        for (const auto& muted : itr->value.GetArray())
            out.emplace_back(muted.GetString(), muted.GetStringLength());
        return true;
    }
    return false;
}

bool LoadConfig() {
    INFO("Loading Config...");
    bool foundEverything = true;
    auto& cfg = get_config().config;

    // Text
    GET(enableTextchat);
    GET(enablePlayerBubbles);
    GET(enableCenterBubbles);
    GET(soundNotification);
    GET(soundNotificationVolume);

    // Voice
    GET(enableVoiceChat);
    GET(microphoneDevice);
    GET(jitterBufferMs);
    GET(spatialBlend);
    GET(voiceActivationMode);
    GET(voiceKeybind);
    GET(voiceKeybindController);

    // HUD
    GET(enableHud);
    GET(hudOpacity);
    GET(hudOffsetCamX);
    GET(hudOffsetCamY);
    GET(hudOffsetCamZ);

    // Shared
    GET(mutedUserIds);

    if (foundEverything) INFO("Config Loaded!");
    return foundEverything;
}
