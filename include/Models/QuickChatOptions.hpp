#pragma once

#include "custom-types/shared/macros.hpp"
#include "Zenject/IInitializable.hpp"

#include <unordered_map>
#include <string>
DECLARE_CLASS_CODEGEN_INTERFACES(MultiplayerChat::Models, QuickChatOptions, Il2CppObject, classof(Zenject::IInitializable*),
    DECLARE_OVERRIDE_METHOD(void, Initialize, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::Zenject::IInitializable::Initialize>::get());
    DECLARE_CTOR(ctor);

    public:
        static constexpr const char* QuickChatOptionsPath = "/sdcard/ModData/com.beatgames.beatsaber/Mods/MultiplayerChat/quickchat.json";
        const std::unordered_map<std::string, std::vector<std::string>>& get_options() const { return _options; }
    private:
        void WriteOutDefault();
        std::unordered_map<std::string, std::vector<std::string>> _options;
)
