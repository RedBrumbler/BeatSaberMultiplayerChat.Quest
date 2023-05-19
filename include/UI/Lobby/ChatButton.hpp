#pragma once

#include "custom-types/shared/macros.hpp"
#include "lapiz/shared/macros.hpp"

#include "Utilities/SpriteManager.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "Zenject/DiContainer.hpp"
#include "HMUI/ImageView.hpp"

DECLARE_CLASS_CODEGEN(MultiplayerChat::UI::Lobby, ChatButton, UnityEngine::MonoBehaviour,
    DECLARE_INSTANCE_FIELD_PRIVATE(Zenject::DiContainer*, _diContainer);
    DECLARE_INSTANCE_FIELD_PRIVATE(Utilities::SpriteManager*, _spriteManager);
    DECLARE_INSTANCE_FIELD_PRIVATE(HMUI::ImageView*, _unreadBadge);

    DECLARE_INSTANCE_METHOD(void, Awake);
    DECLARE_INJECT_METHOD(void, Inject, Zenject::DiContainer* container, Utilities::SpriteManager* spriteManager);

    DECLARE_CTOR(ctor);
    public:
        UnorderedEventCallback<> onClick;
        static ChatButton* Create(Zenject::DiContainer* container);
        void ShowUnread();
        void HideUnread();
)
