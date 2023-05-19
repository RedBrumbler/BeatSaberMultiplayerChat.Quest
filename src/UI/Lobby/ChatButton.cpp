#include "UI/Lobby/ChatButton.hpp"

#include "custom-types/shared/delegate.hpp"
#include "HMUI/HoverHint.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/RectTransform.hpp"
#include "UnityEngine/UI/Button.hpp"
#include "UnityEngine/UI/Button_ButtonClickedEvent.hpp"
#include "UnityEngine/Events/UnityAction.hpp"

DEFINE_TYPE(MultiplayerChat::UI::Lobby, ChatButton);

namespace MultiplayerChat::UI::Lobby {
    void ChatButton::ctor() {
        INVOKE_CTOR();
    }

    void ChatButton::Inject(Zenject::DiContainer* container, Utilities::SpriteManager* spriteManager) {
        _diContainer = container;
        _spriteManager = spriteManager;
    }

    ChatButton* ChatButton::Create(Zenject::DiContainer* container) {
        auto titleViewTransform = UnityEngine::GameObject::Find("Wrapper/MenuCore/UI/ScreenSystem/TopScreen/TitleViewController")->get_transform();
        auto backButton = titleViewTransform->Find("BackButton");

        auto cloneButton = Instantiate(backButton->get_gameObject(), titleViewTransform);
        cloneButton->set_name("ChatButton");
        cloneButton->get_transform()->SetAsLastSibling();
        return container->InstantiateComponent<ChatButton*>(cloneButton);
    }

    void ChatButton::Awake() {
        auto hoverHint = GetComponent<HMUI::HoverHint*>();
        if (!hoverHint) hoverHint = _diContainer->InstantiateComponent<HMUI::HoverHint*>(get_gameObject());

        hoverHint->set_text("Multiplayer Chat");

        auto icon = get_transform()->Find("Icon")->GetComponent<HMUI::ImageView*>();
        icon->set_sprite(_spriteManager->get_chatIcon());

        auto rectTransform = reinterpret_cast<UnityEngine::RectTransform*>(get_transform());

        auto localPosition = rectTransform->get_localPosition();
        localPosition.x = 68;
        rectTransform->set_localPosition(localPosition);

        // Bind action
        GetComponent<UnityEngine::UI::Button*>()->get_onClick()->AddListener(
            custom_types::MakeDelegate<UnityEngine::Events::UnityAction*>(std::function<void()>(
                [this](){ onClick.invoke(); }
            )));

        // Add Unread badge
        auto unreadBase = UnityEngine::Object::Instantiate(icon)->get_transform();
        unreadBase->set_name("UnreadBadge");
        unreadBase->SetParent(get_transform(), false);
        unreadBase->SetAsLastSibling();
        _unreadBadge = unreadBase->get_gameObject()->GetComponent<HMUI::ImageView*>();
        _unreadBadge->set_sprite(_spriteManager->get_unreadBadgeIcon());
        unreadBase->set_localPosition({-10, 3.33f, 0});
        HideUnread();
    }

    void ChatButton::ShowUnread() {
        if (!_unreadBadge || !_unreadBadge->m_CachedPtr.m_value) return;
        _unreadBadge->get_gameObject()->SetActive(true);
    }

    void ChatButton::HideUnread() {
        if (!_unreadBadge || !_unreadBadge->m_CachedPtr.m_value) return;
        _unreadBadge->get_gameObject()->SetActive(false);
    }
}
