#pragma once

#include "custom-types/shared/macros.hpp"
#include "custom-types/shared/coroutine.hpp"

#include "UnityEngine/MonoBehaviour.hpp"
#include "Zenject/DiContainer.hpp"
#include "UnityEngine/Transform.hpp"

#include "UnityEngine/RectTransform.hpp"
#include "UnityEngine/CanvasGroup.hpp"
#include "UnityEngine/Vector3.hpp"
#include "HMUI/ImageView.hpp"
#include "HMUI/CurvedTextMeshPro.hpp"

namespace MultiplayerChat::UI {
    enum class AlignStyle {
        LobbyAvatar,
        CenterScreen
    };

    enum class InnerState {
        Idle,
        AnimateIn,
        ShowWait,
        AnimateOut
    };
}

DECLARE_CLASS_CODEGEN(MultiplayerChat::UI, ChatBubble, UnityEngine::MonoBehaviour,
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::RectTransform*, _rectTransform);
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::CanvasGroup*, _canvasGroup);
    DECLARE_INSTANCE_FIELD_PRIVATE(HMUI::ImageView*, _bg);
    DECLARE_INSTANCE_FIELD_PRIVATE(HMUI::CurvedTextMeshPro*, _textMesh);
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::Vector3, _localPosTarget);

    DECLARE_INSTANCE_METHOD(void, Awake);
    DECLARE_CTOR(ctor);
    public:
        static ChatBubble* Create(Zenject::DiContainer* container, UnityEngine::Transform* parent, AlignStyle style);
        bool get_isShowing() const { return _state != InnerState::Idle; }

        void Show(std::string text);
        void RefreshSize();
        void HideAnimated();
        void HideImmediate();
    private:
        static constexpr const UnityEngine::Vector2 Padding = UnityEngine::Vector2(8, 4);
        static constexpr const float YOffsetCenterScreen = 60;
        static constexpr const float YOffsetPlayerAvatar = 20;
        static constexpr const float ZOffset = -0.1f;

        static constexpr const float AnimationDuration = 0.15f;
        static constexpr const float AnimationOffsetY = -4;
        static constexpr const float DisplayTime = 5;

        custom_types::Helpers::Coroutine AnimateInRoutine();
        custom_types::Helpers::Coroutine AnimateOutRoutine();

        AlignStyle _style = AlignStyle::CenterScreen;
        InnerState _state = InnerState::Idle;
)
