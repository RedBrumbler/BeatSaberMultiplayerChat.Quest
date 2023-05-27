#pragma once

#include "custom-types/shared/macros.hpp"
#include "UnityEngine/Sprite.hpp"

#include "Zenject/IInitializable.hpp"
#include "System/IDisposable.hpp"

DECLARE_CLASS_CODEGEN_INTERFACES(MultiplayerChat::Utilities, SpriteManager, Il2CppObject, std::vector<Il2CppClass*>({classof(Zenject::IInitializable*), classof(System::IDisposable*)}),
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::Sprite*, _chatIcon);
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::Sprite*, _keyboardIcon);
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::Sprite*, _micOffIcon);
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::Sprite*, _micOnIcon);
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::Sprite*, _unreadBadgeIcon);
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::Sprite*, _bgMask);
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::Sprite*, _radialButton);

    DECLARE_OVERRIDE_METHOD(void, Initialize, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::Zenject::IInitializable::Initialize>::get());
    DECLARE_OVERRIDE_METHOD(void, Dispose, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::System::IDisposable::Dispose>::get());

    DECLARE_DEFAULT_CTOR();

    public:
        UnityEngine::Sprite* get_chatIcon() const { return _chatIcon; }
        UnityEngine::Sprite* get_keyboardIcon() const { return _keyboardIcon; }
        UnityEngine::Sprite* get_micOffIcon() const { return _micOffIcon; }
        UnityEngine::Sprite* get_micOnIcon() const { return _micOnIcon; }
        UnityEngine::Sprite* get_unreadBadgeIcon() const { return _unreadBadgeIcon; }
        UnityEngine::Sprite* get_bgMask() const { return _bgMask; }
        UnityEngine::Sprite* get_radialButton() const { return _radialButton; }
)
