#include "UI/Hud/HudVoiceIndicator.hpp"
#include "UI/MpcColors.hpp"
#include "config.hpp"

#include "custom-types/shared/delegate.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/CanvasRenderer.hpp"
#include "UnityEngine/Canvas.hpp"
#include "UnityEngine/RenderMode.hpp"
#include "UnityEngine/CanvasGroup.hpp"
#include "UnityEngine/UI/CanvasScaler.hpp"
#include "UnityEngine/Quaternion.hpp"
#include "UnityEngine/Vector3.hpp"
#include "HMUI/CurvedCanvasSettings.hpp"

DEFINE_TYPE(MultiplayerChat::UI::Hud, HudVoiceIndicator);

static UnityEngine::Quaternion operator *(UnityEngine::Quaternion a, UnityEngine::Quaternion b) {
    return UnityEngine::Quaternion::op_Multiply(a, b);
}

static constexpr inline UnityEngine::Color lerp(UnityEngine::Color a, UnityEngine::Color b, float t) {
    return {
        a.r + (b.r - a.r) * t,
        a.g + (b.g - a.g) * t,
        a.b + (b.b - a.b) * t,
        a.a + (b.a - a.a) * t,
    };
}

namespace MultiplayerChat::UI::Hud {
    void HudVoiceIndicator::ctor() {
        INVOKE_CTOR();

        _connectedAction = custom_types::MakeDelegate<decltype(___backing_field__connectedAction)>(std::function<void()>(
            std::bind(&HudVoiceIndicator::RefreshStatus, this)
        ));

        _disconnectedAction = custom_types::MakeDelegate<decltype(___backing_field__disconnectedAction)>(std::function<void(GlobalNamespace::DisconnectedReason)>(
            std::bind(&HudVoiceIndicator::RefreshStatus_Disconnected, this, std::placeholders::_1)
        ));
    }

    void HudVoiceIndicator::Inject(Utilities::SpriteManager* spriteManager, Audio::MicrophoneManager* microphoneManager, GlobalNamespace::IMultiplayerSessionManager* sessionManager, Audio::VoiceManager* voiceManager, Core::InputManager* inputManager) {
        _spriteManager = spriteManager;
        _microphoneManager = microphoneManager;
        _sessionManager = sessionManager;
        _voiceManager = voiceManager;
        _inputManager = inputManager;
    }

    void HudVoiceIndicator::Awake() {
        auto gameObject = get_gameObject();
        gameObject->set_layer(5); // UI

        auto canvas = gameObject->AddComponent<UnityEngine::Canvas*>();
        canvas->set_renderMode(UnityEngine::RenderMode::WorldSpace);
        canvas->set_scaleFactor(1);
        canvas->set_referencePixelsPerUnit(1);

        using SetPlaneDistance_fun = function_ptr_t<void, UnityEngine::Canvas*, float>;
        static auto SetPlaneDistance = reinterpret_cast<SetPlaneDistance_fun>(il2cpp_functions::resolve_icall("UnityEngine.Canvas::set_planeDistance"));
        SetPlaneDistance(canvas, 100.0f);

        gameObject->AddComponent<UnityEngine::CanvasRenderer*>();
        gameObject->AddComponent<UnityEngine::UI::CanvasScaler*>();
        gameObject->AddComponent<HMUI::CurvedCanvasSettings*>();

        _canvasGroup = gameObject->AddComponent<UnityEngine::CanvasGroup*>();
        _canvasGroup->set_alpha(0);

        auto bg = UnityEngine::GameObject::New_ctor("BG");
        bg->get_transform()->SetParent(get_transform());
        bg->AddComponent<UnityEngine::CanvasRenderer*>();

        _bgImage = bg->AddComponent<UnityEngine::UI::Image*>();
        _bgImage->set_sprite(_spriteManager->get_micOnIcon());
        _bgImage->set_color(MpcColors::Red);

        get_transform()->set_localScale({.0005f, .0005f, .0005f});

        _currentDisplayState = DisplayState::Hidden;
        _targetDisplayState = DisplayState::Hidden;

        _mainCamera = UnityEngine::Camera::get_main();
    }

    void HudVoiceIndicator::OnEnable() {
        _sessionManager->add_connectedEvent(_connectedAction);
        _sessionManager->add_disconnectedEvent(_disconnectedAction);

        _inputManager->activatedEvent += {&HudVoiceIndicator::RefreshStatus, this};
        _inputManager->deactivatedEvent += {&HudVoiceIndicator::RefreshStatus, this};
        _inputManager->testModeChangedEvent += {&HudVoiceIndicator::RefreshStatus, this};

        _voiceManager->startedTransmittingEvent += {&HudVoiceIndicator::RefreshStatus, this};
        _voiceManager->stoppedTransmittingEvent += {&HudVoiceIndicator::RefreshStatus, this};

        RefreshStatus();
    }

    void HudVoiceIndicator::OnDisable() {
        _sessionManager->remove_connectedEvent(_connectedAction);
        _sessionManager->remove_disconnectedEvent(_disconnectedAction);

        _inputManager->activatedEvent -= {&HudVoiceIndicator::RefreshStatus, this};
        _inputManager->deactivatedEvent -= {&HudVoiceIndicator::RefreshStatus, this};
        _inputManager->testModeChangedEvent -= {&HudVoiceIndicator::RefreshStatus, this};

        _voiceManager->startedTransmittingEvent -= {&HudVoiceIndicator::RefreshStatus, this};
        _voiceManager->stoppedTransmittingEvent -= {&HudVoiceIndicator::RefreshStatus, this};
    }

#pragma region State
    void HudVoiceIndicator::RefreshStatus() {

        auto isPlayingOrTesting = _sessionManager->get_isConnected() || _inputManager->get_testMode();
        auto canActivate = _voiceManager->get_canTransmit()|| _inputManager->get_testMode();

        if (!config.enableVoiceChat || !config.enableHud || !isPlayingOrTesting) {
            // Voice and/or HUD is completely disabled, or not in a MP session - hide completely
            _targetDisplayState = DisplayState::Hidden;
        } else if (!_microphoneManager->get_haveSelectedDevice()|| !canActivate) {
            // No mic selected, or cannot transmit right now - show as locked
            _targetDisplayState = DisplayState::VisibleLocked;
        } else if (_voiceManager->get_isTransmitting() || _voiceManager->get_isLoopbackTesting()) {
            // Actively transmitting - show as active
            _targetDisplayState = DisplayState::VisibleActive;
        } else {
            // Not transmitting - show as muted
            _targetDisplayState = DisplayState::VisibleMuted;
        }
    }

    void HudVoiceIndicator::RefreshStatus_Disconnected(GlobalNamespace::DisconnectedReason reason) { RefreshStatus(); }
#pragma endregion // State

#pragma region UI Update
    void HudVoiceIndicator::Update() {
        if (_inputManager->get_testMode())
            // Test mode - settings open, keep refreshing status
            RefreshStatus();

        if (_currentDisplayState == DisplayState::Hidden && _targetDisplayState == DisplayState::Hidden)
            // Fully hidden, do nothing
            return;

        if (_mainCamera && _mainCamera->m_CachedPtr) {
            // Stick to main cam
            auto selfTransform = get_transform();
            selfTransform->set_position(_mainCamera->ViewportToWorldPoint(get_hudOffset(), UnityEngine::Camera::MonoOrStereoscopicEye::Mono));
            selfTransform->set_rotation(_mainCamera->get_transform()->get_rotation() * get_baseRotation());
        }

        if (!_inputManager->get_testMode() && _currentDisplayState == _targetDisplayState)
            // No transition needed
            return;

        _currentDisplayState = DisplayState::Transitioning;

        auto opacityOk = false;
        auto colorOk = false;

        float targetOpacity;
        UnityEngine::Color targetColor;
        UnityEngine::Sprite* targetSprite;

        switch (_targetDisplayState)
        {
            using enum DisplayState;
            case Hidden:
                targetOpacity = 0;
                targetColor = MpcColors::Red;
                targetSprite = _spriteManager->MicOffIcon;
                break;
            default:
            case VisibleActive:
                targetOpacity = config.hudOpacity;
                targetColor = MpcColors::Green;
                targetSprite = _spriteManager->MicOnIcon;
                break;
            case VisibleMuted:
            case VisibleLocked:
                targetOpacity = config.hudOpacity * .5f;
                targetColor = MpcColors::Red;
                targetSprite = _spriteManager->MicOffIcon;
                break;
        }

        if (_canvasGroup) {
            // Fade to target opacity
            _canvasGroup->set_alpha(std::lerp(_canvasGroup->get_alpha(), targetOpacity, TransitionLerp));
            if (_canvasGroup->get_alpha() - targetOpacity < 0.002f)
                opacityOk = true;
        }

        if (_bgImage) {
            // Fade to target color
            _bgImage->set_color(lerp(_bgImage->get_color(), targetColor, TransitionLerp));

            if (MpcColors::IsColorVeryCloseToColor(_bgImage->get_color(), targetColor))
                colorOk = true;

            // Update sprite
            if (targetSprite && _bgImage->get_sprite()->get_name() != targetSprite->get_name())
                _bgImage->set_sprite(targetSprite);
        }

        if (opacityOk && colorOk) {
            // Transition complete
            _currentDisplayState = _targetDisplayState;
        }
    }

    UnityEngine::Vector3 HudVoiceIndicator::get_hudOffset() { return {config.hudOffsetCamX, config.hudOffsetCamY, config.hudOffsetCamZ}; }
    float HudVoiceIndicator::get_hudOffsetCamX() { return config.hudOffsetCamX; }
    float HudVoiceIndicator::get_hudOffsetCamY() { return config.hudOffsetCamY; }
    float HudVoiceIndicator::get_hudOffsetCamZ() { return config.hudOffsetCamZ; }

    UnityEngine::Quaternion HudVoiceIndicator::get_baseRotation() {
        static auto baseRotation = UnityEngine::Quaternion::Euler(15, 15, 0);
        return baseRotation;
    }

#pragma endregion // UI Update
}
