#include "UI/Lobby/ChatViewController.hpp"

#include "assets.hpp"
#include "bsml/shared/BSML.hpp"

#include "custom-types/shared/delegate.hpp"
#include "UnityEngine/Events/UnityAction.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/UI/LayoutElement.hpp"
#include "UnityEngine/UI/ContentSizeFitter.hpp"
#include "UnityEngine/UI/Button.hpp"
#include "UnityEngine/UI/Button_ButtonClickedEvent.hpp"
#include "UnityEngine/UI/HorizontalLayoutGroup.hpp"
#include "HMUI/CurvedTextMeshPro.hpp"

#define protected public
#include "bsml/shared/BSML/Tags/ImageTag.hpp"
#include "bsml/shared/BSML/Tags/HorizontalTag.hpp"
#include "bsml/shared/BSML/Tags/TextTag.hpp"
#undef protected

DEFINE_TYPE(MultiplayerChat::UI::Lobby, ChatViewController);

namespace MultiplayerChat::UI::Lobby {
    void ChatViewController::ctor() {
        INVOKE_CTOR();
    }

    void ChatViewController::Inject(Core::ChatManager* chatManager) {
        _chatManager = chatManager;
    }

#pragma region Core Events
    void ChatViewController::PostParse() {
        if (scrollableContainer)
            scrollableContainerContent = scrollableContainer->get_transform()->Find("Viewport/Content Wrapper");

        _bsmlReady = true;

        scrollableContainer->pageUpButton->get_onClick()->AddListener(custom_types::MakeDelegate<UnityEngine::Events::UnityAction*>(std::function<void()>(
            std::bind(&ChatViewController::HandleScrollablePageUp, this)
        )));
        scrollableContainer->pageDownButton->get_onClick()->AddListener(custom_types::MakeDelegate<UnityEngine::Events::UnityAction*>(std::function<void()>(
            std::bind(&ChatViewController::HandleScrollablePageDown, this)
        )));

        ApplyUIMutations();
        FillChat();

        chatInput->modalKeyboard->keyboard->enterPressed = std::bind(&ChatViewController::HandleKeyboardInput, this, std::placeholders::_1);
    }

    void ChatViewController::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
        BSML::parse_and_construct(IncludedAssets::ChatViewController_bsml, get_transform(), this);

        if (!_bsmlReady || firstActivation) return;

        FillChat();
        ResetChatInputText();
    }

    void ChatViewController::HandleKeyboardInput(StringW input) {
        // because on enter is not an event, we have to call what it would normally call
        chatInput->modalKeyboard->OnEnter(input);

        StartCoroutine(custom_types::Helpers::CoroutineHelper::New(HandleKeyboardInputRoutine(input)));
    }

    custom_types::Helpers::Coroutine ChatViewController::HandleKeyboardInputRoutine(StringW input) {
        co_yield nullptr;

        ResetChatInputText();
        input = input->Trim();

        if (Il2CppString::IsNullOrWhiteSpace(input)) co_return;

        _chatManager->SendTextChat(input);

        co_return;
    }

#pragma endregion // Core Events

#pragma region Core UI
    void ChatViewController::ApplyUIMutations() {
        if (!chatViewBg || !chatInput) return;

        // Remove skew from main chat background
        auto bgImage = chatViewBg->GetComponent<HMUI::ImageView*>();
        bgImage->skew = 0;
        bgImage->__Refresh();

        // Make the keyboard input look nice
        // > Remove the label
        auto inputT = chatInput->get_transform();
        inputT->Find("NameText")->get_gameObject()->SetActive(false);

        // > Stretch the input element to span the full width
        auto valuePickerRect = reinterpret_cast<UnityEngine::RectTransform*>(inputT->Find("ValuePicker"));
        valuePickerRect->set_offsetMin({-105, 0});

        // > Make the background look nice
        auto buttonLeftSide = reinterpret_cast<UnityEngine::RectTransform*>(valuePickerRect->Find("DecButton"));
        auto buttonRightSide = reinterpret_cast<UnityEngine::RectTransform*>(valuePickerRect->Find("IncButton"));
        auto valueText = reinterpret_cast<UnityEngine::RectTransform*>(valuePickerRect->Find("ValueText"));

        const float leftSideWidth = 0.05f;

        buttonLeftSide->set_anchorMin({0.0f, 0.0f});
        buttonLeftSide->set_anchorMax({leftSideWidth, 1.0f});
        buttonLeftSide->set_offsetMin({0.0f, 0.0f});
        buttonLeftSide->set_offsetMax({0.0f, 0.0f});
        buttonLeftSide->set_sizeDelta({0.0f, 0.0f});

        buttonRightSide->set_anchorMin({leftSideWidth, 0.0f});
        buttonRightSide->set_anchorMax({1.0f, 1.0f});
        buttonRightSide->set_offsetMin({0.0f, 0.0f});
        buttonRightSide->set_offsetMax({0.0f, 0.0f});
        buttonRightSide->set_sizeDelta({0.0f, 0.0f});

        valueText->set_anchorMin({0.0f, 0.0f});
        valueText->set_anchorMax({1.0f, 1.0f});
        valueText->set_offsetMin({0.0f, -0.33f});
        valueText->set_offsetMax({0.0f, 0.0f});
        valueText->set_sizeDelta({0.0f, 0.0f});

        // > Remove skew from backgrounds
        auto bgLeft = buttonLeftSide->Find("BG")->GetComponent<HMUI::ImageView*>();
        bgLeft->skew = 0;
        bgLeft->__Refresh();

        auto bgRight = buttonRightSide->Find("BG")->GetComponent<HMUI::ImageView*>();
        bgRight->skew = 0;
        bgRight->__Refresh();

        // > Remove ugly edit icon
        buttonRightSide->Find("EditIcon")->get_gameObject()->SetActive(false);

        // > Make placeholder text look like placeholder text
        auto valueTextMesh = valueText->GetComponent<HMUI::CurvedTextMeshPro*>();
        valueTextMesh->set_alignment(TMPro::TextAlignmentOptions::Center);
        valueTextMesh->set_color({0.5, 0.5, 0.5, 1});

        ResetChatInputText();
    }

    void ChatViewController::ResetChatInputText() {
        chatInput->set_text("");
        chatInput->text->set_text(ChatInputPlaceholderText);
    }

#pragma endregion // Core UI

#pragma region Messages data/rendering
    void ChatViewController::FillChat() {
        ClearMessages(true);

        for (const auto& message : _messageBuffer)
            AddMessage(message, true);

        _chatLockedToBottom = true;
    }

    void ChatViewController::ClearMessages(bool visualOnly) {
        if (!visualOnly)
            _messageBuffer.clear();

        if (!scrollableContainerContent) return;

        auto contentT = scrollableContainerContent->get_transform();
        int childCount = contentT->get_childCount();

        for (int i = childCount - 1; i >= 0; i--)
            UnityEngine::Object::DestroyImmediate(contentT->GetChild(i)->get_gameObject());

        scrollableContainer->RefreshContent();
    }

    void ChatViewController::AddMessage(const Models::ChatMessage& message, bool visualOnly) {
        if (!visualOnly) {
            if (_messageBuffer.size() > MaxBufferSize)
                _messageBuffer.erase(_messageBuffer.begin());

            _messageBuffer.push_back(message);
        }

        if (!get_gameObject()->get_activeInHierarchy() || !_bsmlReady) return;

        if (!scrollableContainer) return;

        auto [text, image] = AddMessageObject();

        // FIXME: messages that are too long without newlines seem to spill over into the next message, find a way to prevent that (or add newlines)
        text->set_text(message.FormatMessage(false, true));
        text->ForceMeshUpdate();

        image->set_sprite(message.SpriteForMessage());

        _chatLockedToBottom = true;
    }

    std::pair<TMPro::TextMeshProUGUI*, HMUI::ImageView*> ChatViewController::AddMessageObject() {
        auto layoutGo = BSML::HorizontalTag().CreateObject(scrollableContainerContent);
        auto layout = layoutGo->GetComponent<UnityEngine::UI::HorizontalLayoutGroup*>();

        auto imageGo = BSML::ImageTag().CreateObject(layoutGo->get_transform());
        auto imageRect = reinterpret_cast<UnityEngine::RectTransform*>(imageGo->get_transform());
        imageRect->set_pivot({0, 1});
        imageRect->set_anchorMin({0, 1});
        imageRect->set_anchorMax({0, 1});
        imageRect->set_sizeDelta({5, 5});

        auto layoutElement = imageGo->GetComponent<UnityEngine::UI::LayoutElement*>();
        layoutElement->set_ignoreLayout(true);

        // set image data
        auto imageView = imageGo->GetComponent<HMUI::ImageView*>();
        imageView->set_preserveAspect(true);
        imageView->skew = 0;
        imageView->__Refresh();

        // create text
        auto textGo = BSML::TextTag().CreateObject(layoutGo->get_transform());
        auto textComponent = textGo->GetComponent<TMPro::TextMeshProUGUI*>();

        textComponent->set_text("");
        textComponent->set_fontSize(3.4f);
        textComponent->set_richText(true);
        textComponent->set_enableWordWrapping(true);
        return {textComponent, imageView};
    }

#pragma endregion // Messages data/rendering

#pragma region Scroll pain
    void ChatViewController::HandleScrollablePageUp() {
        _chatLockedToBottom = false;
    }

    void ChatViewController::HandleScrollablePageDown() {
        if (!scrollableContainer) return;
        _chatLockedToBottom = !scrollableContainer->pageDownButton->get_interactable();
    }

#pragma endregion // Scroll pain
}
