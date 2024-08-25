#include "UI/Lobby/QuickChatModal.hpp"
#include "assets.hpp"

#include "bsml/shared/BSML.hpp"
#include "GlobalNamespace/ExtensionMethods.hpp"
#include "UnityEngine/RectTransform.hpp"

#define protected public
#include "bsml/shared/BSML/Tags/ClickableImageTag.hpp"
#include "bsml/shared/BSML/Tags/TextTag.hpp"
#undef protected

DEFINE_TYPE(MultiplayerChat::UI::Lobby, QuickChatModal);

namespace MultiplayerChat::UI::Lobby {
    void QuickChatModal::ctor() {
        INVOKE_CTOR();
        INVOKE_BASE_CTOR(classof(UnityEngine::MonoBehaviour*));
    }

    void QuickChatModal::Inject(Core::ChatManager* chatManager, Models::QuickChatOptions* options, Utilities::SpriteManager* spriteManager) {
        _chatManager = chatManager;
        _options = options;
        _spriteManager = spriteManager;
    }

    void QuickChatModal::Initialize() {
        BSML::parse_and_construct(Assets::QuickChatModal_bsml, get_transform(), this);
    }

    void QuickChatModal::Dispose() {
        if (modal && modal->m_CachedPtr.m_value) {
            Destroy(modal->get_gameObject());
        }
    }

#pragma region Show/Hide API
    void QuickChatModal::Show() {
        UpdateButtonTexts();
        modal->Show();
    }

    void QuickChatModal::Hide(bool immediately) {
        modal->HMUI::ModalView::Hide(!immediately, nullptr);
    }

#pragma endregion // Show/Hide API

#pragma region UI handling
    void QuickChatModal::PostParse() {
        // our event
        modal->onHide = std::bind(&QuickChatModal::BlockerClicked, this);

        // change the background sprite to be an octagon
        auto bg = modal->GetComponentInChildren<HMUI::ImageView*>(true);
        bg->set_sprite(_spriteManager->get_bgMask());

        // create all the buttons & save their texts
        _buttonTexts = CreateButtonWheel(modal->get_transform());

        // fixup back button
        backButton->get_transform()->set_localScale({0.5, 0.5, 0.5});
        backButton->set_defaultColor({1, 1, 1, 0.2});
        backButton->set_highlightColor({1, 1, 1, 1});

        // update buttons text
        UpdateButtonTexts();
    }

    ArrayW<TMPro::TextMeshProUGUI*> QuickChatModal::CreateButtonWheel(UnityEngine::Transform* parent) {
        // result
        auto texts = ArrayW<TMPro::TextMeshProUGUI*>(il2cpp_array_size_t(8));

        // used to create items
        auto imageTag = BSML::ClickableImageTag();
        auto textTag = BSML::TextTag();

        // base values
        auto offsetVector = UnityEngine::Vector2(0, 27.5f);
        auto center = UnityEngine::Vector2(0, 0);

        // load the sprite
        auto radialButtonSprite = _spriteManager->get_radialButton();

        static constexpr auto deg2rad = (2 * 3.141592654) / 360.0f;

        for (int i = 0; i < 8; i++) {
            auto btn = imageTag.CreateObject(parent)->GetComponent<BSML::ClickableImage*>();

            // positioning & rotation
            float unitAngle = 1.0f / 16.0f + float(i) / 8.0f;
            auto btnT = btn->get_rectTransform();
            auto centerAdd = GlobalNamespace::ExtensionMethods::Rotate(offsetVector, (unitAngle * 360.0f * deg2rad));
            center.x += centerAdd.x;
            center.y += centerAdd.y;
            btnT->set_anchoredPosition(center);
            btnT->set_localRotation(UnityEngine::Quaternion::Euler(0, 0, unitAngle * 360));
            btnT->set_localScale({1.25, 1.25, 1.25});

            // change the look of the button
            btn->set_sprite(radialButtonSprite);
            btn->set_defaultColor({1, 1, 1, 0.2f});
            btn->set_highlightColor({1, 1, 1, 1});
            btn->set_preserveAspect(true);

            // subscribe event
            btn->onClick += [this, idx = i](){ IndexedButtonClicked(idx); };

            // add button text
            auto txt = textTag.CreateObject(btn->get_transform())->GetComponent<TMPro::TextMeshProUGUI*>();
            texts[i] = txt;

            // position text
            auto txtT = txt->get_rectTransform();
            txtT->set_anchoredPosition({0, 0});
            txtT->set_anchorMin({0, 0});
            txtT->set_anchorMax({1, 1});
            txtT->set_offsetMin({0, 0});
            txtT->set_offsetMax({0, 0});
            txtT->set_localScale({0.5, 0.5, 0.5});

            // rotate text back so it's upright
            txtT->set_localRotation(UnityEngine::Quaternion::Euler(0, 0, -unitAngle * 360));

            // clear text & align
            txt->set_alignment(TMPro::TextAlignmentOptions::Center);
            txt->set_text("");
        }

        return texts;
    }

    void QuickChatModal::UpdateButtonTexts() {
        auto& texts = get_buttonTexts();
        auto textsLen = texts.size();
        for (int i = 0; i < 8; i++) {
            auto btnText = _buttonTexts[i];
            btnText->set_text(i < textsLen ? texts[i] : "");
            btnText->get_transform()->get_parent()->get_gameObject()->SetActive(!System::String::IsNullOrEmpty(btnText->get_text()));
        }
    }

#pragma endregion // UI handling

#pragma region UI events
    void QuickChatModal::BackButtonClick() {
        if (!get_inSubMenu()) Hide();

        _currentMenu = RootMenu;
        UpdateButtonTexts();
    }

    void QuickChatModal::BlockerClicked() {
        _currentMenu = RootMenu;
        BackButtonClick();
    }

    void QuickChatModal::IndexedButtonClicked(int idx) {
        if (get_inSubMenu()) {
            try {
                _chatManager->SendTextChat(get_buttonTexts().at(idx));
            } catch(const std::out_of_range& e) {
                ERROR("Error thrown while trying to send chat: {}", e.what());
            } catch(...) {}

            Hide();
            _currentMenu = RootMenu;
        } else {
            try {
                _currentMenu = get_buttonTexts().at(idx);
            } catch(const std::out_of_range& e) {
                ERROR("Error thrown while trying to send chat: {}", e.what());
                _currentMenu = RootMenu;
            } catch (...) {
                _currentMenu = RootMenu;
            }
        }
        UpdateButtonTexts();
    }

#pragma endregion // UI events

}
