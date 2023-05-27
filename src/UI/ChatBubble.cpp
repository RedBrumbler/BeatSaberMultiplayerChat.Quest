#include "UI/ChatBubble.hpp"

#include "UnityEngine/WaitForSeconds.hpp"
#include "UnityEngine/WaitForEndOfFrame.hpp"
#include "UnityEngine/Time.hpp"
#include "UnityEngine/Bounds.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/Canvas.hpp"
#include "HMUI/HoverHintController.hpp"
#include "HMUI/HoverHintPanel.hpp"

DEFINE_TYPE(MultiplayerChat::UI, ChatBubble);

namespace MultiplayerChat::UI {
    void ChatBubble::ctor() {
        INVOKE_CTOR();
    }
    ChatBubble* ChatBubble::Create(Zenject::DiContainer* container, UnityEngine::Transform* parent, AlignStyle style) {
        auto hhc = container->Resolve<HMUI::HoverHintController*>();
        auto hhPrefab = hhc->hoverHintPanelPrefab;

        auto instance = UnityEngine::Object::Instantiate(hhPrefab->get_gameObject(), parent, false);
        instance->get_transform()->SetAsLastSibling();
        instance->set_name("MpcChatBubble");

        UnityEngine::Object::Destroy(instance->GetComponent<HMUI::HoverHintPanel*>());

        auto bubble = container->InstantiateComponent<ChatBubble*>(instance);
        bubble->_style = style;
        return bubble;
    }

    void ChatBubble::Awake() {
        _rectTransform = reinterpret_cast<UnityEngine::RectTransform*>(get_transform());
        _rectTransform->set_pivot({0.5f, 0.5f});
        _rectTransform->set_anchorMin({0.5f, 0.5f});
        _rectTransform->set_anchorMax({0.5f, 0.5f});

        _canvasGroup = GetComponent<UnityEngine::CanvasGroup*>();
        if (!_canvasGroup) _canvasGroup = get_gameObject()->AddComponent<UnityEngine::CanvasGroup*>();

        _bg = GetComponent<HMUI::ImageView*>();
        _bg->set_color({0, 0, 0, 0.95f});

        _textMesh = get_transform()->Find("Text")->GetComponent<HMUI::CurvedTextMeshPro*>();
        _textMesh->set_text("!ChatBubble!");
        _textMesh->set_color({1, 1, 1, 1});
        _textMesh->set_fontSize(4.8f);
        _textMesh->set_richText(true);

        auto textMeshRect = reinterpret_cast<UnityEngine::RectTransform*>(_textMesh->get_transform());
        textMeshRect->set_offsetMin({-60, 0});
        textMeshRect->set_offsetMax({60, 0});

        _state = InnerState::Idle;

        RefreshSize();
        HideImmediate();
    }

#pragma region Show/Hide
    void ChatBubble::Show(std::string text) {
        if (!_rectTransform || !_canvasGroup || !_textMesh) return;

        _rectTransform->set_localScale({1, 1, 1});
        _rectTransform->set_localRotation({0, 0, 0, 1});

        _canvasGroup->set_alpha(0);

        get_gameObject()->SetActive(true);
        if (!get_gameObject()->get_activeInHierarchy()) return;

        _textMesh->set_text(text);

        RefreshSize();

        StopAllCoroutines();
        StartCoroutine(custom_types::Helpers::CoroutineHelper::New(AnimateInRoutine()));
    }

    void ChatBubble::RefreshSize() {
        if (!_rectTransform || !_textMesh) return;
        _textMesh->ForceMeshUpdate();

        auto vector = _textMesh->get_bounds().get_size() + UnityEngine::Vector3(Padding.x, Padding.y, 0);
        _rectTransform->set_sizeDelta({vector.x, vector.y});

        auto localPosition = _rectTransform->get_localPosition();
        localPosition.z = ZOffset;
        switch(_style) {
            case AlignStyle::CenterScreen:
                localPosition.y = YOffsetCenterScreen;
                break;
            case AlignStyle::LobbyAvatar:
                localPosition.y = YOffsetPlayerAvatar;
                break;
            default:
                break;
        }

        _rectTransform->set_localPosition(localPosition);
        _localPosTarget = localPosition;

        if (_style == AlignStyle::LobbyAvatar) {
            _rectTransform->set_localScale({2, 2, 2});

            auto canvas = GetComponent<UnityEngine::Canvas*>();
            canvas->set_sortingOrder(0);
            canvas->set_overrideSorting(false);

            // Steal billboard material / font
            auto parentTransform = _rectTransform->get_parent();
            if (parentTransform->get_gameObject()->get_name() != "AvatarCaption")
                return;

            auto avatarCaptionBg = parentTransform->Find("BG")->GetComponent<HMUI::ImageView*>();
            _bg->set_material(avatarCaptionBg->get_material());

            auto avatarCaptionName = parentTransform->Find("Name");
            if (!avatarCaptionName)
                // MpEx nests Name onto BG, see if we can find it there
                auto avatarCaptionName = avatarCaptionBg->get_transform()->Find("Name");

            if (avatarCaptionName) {
                auto avatarCaptionText = avatarCaptionName->GetComponent<HMUI::CurvedTextMeshPro*>();
                _textMesh->set_fontMaterial(avatarCaptionText->get_fontMaterial());
            }
        }

        _textMesh->set_fontStyle(TMPro::FontStyles::Normal);
    }

    void ChatBubble::HideAnimated() {
        if (!get_isActiveAndEnabled()) return;

        if (_state != InnerState::AnimateIn && _state != InnerState::ShowWait) return;

        StopAllCoroutines();
        StartCoroutine(custom_types::Helpers::CoroutineHelper::New(AnimateOutRoutine()));
    }

    void ChatBubble::HideImmediate() {
        if (!get_isActiveAndEnabled()) return;

        StopAllCoroutines();

        get_gameObject()->SetActive(false);

        _state = InnerState::Idle;
    }

#pragma endregion // Show/Hide

#pragma region Animation/Coroutines
    custom_types::Helpers::Coroutine ChatBubble::AnimateInRoutine() {
        if (!_canvasGroup)
            co_return;

        _state = InnerState::AnimateIn;

        float runTime = 0;

        while (runTime < AnimationDuration) {
            runTime += UnityEngine::Time::get_deltaTime();
            auto animationProgress = runTime / AnimationDuration;
            _canvasGroup->set_alpha(animationProgress);

            auto yOffset = AnimationOffsetY - (AnimationOffsetY * animationProgress);
            get_transform()->set_localPosition({
                _localPosTarget.x,
                _localPosTarget.y + yOffset,
                _localPosTarget.z
            });

            co_yield nullptr;
        }

        _canvasGroup->set_alpha(1);
        get_transform()->set_localPosition(_localPosTarget);

        // Wait for some time then animate out
        _state = InnerState::ShowWait;

        co_yield reinterpret_cast<System::Collections::IEnumerator*>(UnityEngine::WaitForSeconds::New_ctor(DisplayTime));

        HideAnimated();
        co_return;
    }

    custom_types::Helpers::Coroutine ChatBubble::AnimateOutRoutine() {
        if (!_canvasGroup) co_return;

        _state = InnerState::AnimateOut;
        float runTime = 0;

        while (runTime < AnimationDuration) {
            runTime += UnityEngine::Time::get_deltaTime();
            auto animationProgress = (runTime / AnimationDuration);

            _canvasGroup->set_alpha(1.0f - animationProgress);

            auto yOffset = (-AnimationOffsetY * animationProgress);
            get_transform()->set_localPosition({
                _localPosTarget.x,
                _localPosTarget.y + yOffset,
                _localPosTarget.z
            });

            co_yield nullptr;
        }

        _canvasGroup->set_alpha(0);

        // Animated out and no longer visible, end of presentation
        co_yield reinterpret_cast<System::Collections::IEnumerator*>(UnityEngine::WaitForEndOfFrame::New_ctor());

        HideImmediate();
        co_return;
    }

#pragma endregion // Animation/Coroutines

}
