#pragma once

#include "custom-types/shared/macros.hpp"
#include "UnityEngine/Sprite.hpp"

#include "Zenject/IInitializable.hpp"
#include "System/IDisposable.hpp"

DECLARE_CLASS_CODEGEN_INTERFACES(MultiplayerChat::Utilities, SpriteManager, System::Object, std::vector<Il2CppClass*>({classof(Zenject::IInitializable*), classof(System::IDisposable*)}),
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::Sprite*, _chatIcon);
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::Sprite*, _keyboardIcon);
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::Sprite*, _micOffIcon);
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::Sprite*, _micOnIcon);
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::Sprite*, _unreadBadgeIcon);
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::Sprite*, _bgMask);
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::Sprite*, _radialButton);

    DECLARE_OVERRIDE_METHOD_MATCH(void, Initialize, &::Zenject::IInitializable::Initialize);
    DECLARE_OVERRIDE_METHOD_MATCH(void, Dispose, &::System::IDisposable::Dispose);

    DECLARE_DEFAULT_CTOR();

    public:

        __declspec(property(get=get_chatIcon)) UnityEngine::Sprite* ChatIcon;
        __declspec(property(get=get_keyboardIcon)) UnityEngine::Sprite* KeyboardIcon;
        __declspec(property(get=get_micOffIcon)) UnityEngine::Sprite* MicOffIcon;
        __declspec(property(get=get_micOnIcon)) UnityEngine::Sprite* MicOnIcon;
        __declspec(property(get=get_unreadBadgeIcon)) UnityEngine::Sprite* UnreadBadgeIcon;
        __declspec(property(get=get_bgMask)) UnityEngine::Sprite* BGMask;
        __declspec(property(get=get_radialButton)) UnityEngine::Sprite* RadialButton;

        UnityEngine::Sprite* get_chatIcon() const { return _chatIcon; }
        UnityEngine::Sprite* get_keyboardIcon() const { return _keyboardIcon; }
        UnityEngine::Sprite* get_micOffIcon() const { return _micOffIcon; }
        UnityEngine::Sprite* get_micOnIcon() const { return _micOnIcon; }
        UnityEngine::Sprite* get_unreadBadgeIcon() const { return _unreadBadgeIcon; }
        UnityEngine::Sprite* get_bgMask() const { return _bgMask; }
        UnityEngine::Sprite* get_radialButton() const { return _radialButton; }
)
