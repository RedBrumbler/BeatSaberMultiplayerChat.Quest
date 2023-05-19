#pragma once

#include "custom-types/shared/macros.hpp"
#include "lapiz/shared/macros.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/Camera.hpp"
#include "UnityEngine/CanvasGroup.hpp"
#include "UnityEngine/UI/Image.hpp"
#include "System/Action.hpp"
#include "System/Action_1.hpp"
#include "GlobalNamespace/DisconnectedReason.hpp"

#include "Utilities/SpriteManager.hpp"
#include "GlobalNamespace/IMultiplayerSessionManager.hpp"
#include "Audio/MicrophoneManager.hpp"
#include "Audio/VoiceManager.hpp"
#include "Core/InputManager.hpp"

namespace MultiplayerChat::UI::Hud {
    enum class DisplayState {
        Transitioning = -1,
        Hidden = 0,
        VisibleLocked = 1,
        VisibleMuted = 2,
        VisibleActive = 3
    };
}

DECLARE_CLASS_CODEGEN(MultiplayerChat::UI::Hud, HudVoiceIndicator, UnityEngine::MonoBehaviour,
    DECLARE_INSTANCE_FIELD_PRIVATE(Utilities::SpriteManager*, _spriteManager);
    DECLARE_INSTANCE_FIELD_PRIVATE(Audio::MicrophoneManager*, _microphoneManager);
    DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::IMultiplayerSessionManager*, _sessionManager);
    DECLARE_INSTANCE_FIELD_PRIVATE(Audio::VoiceManager*, _voiceManager);
    DECLARE_INSTANCE_FIELD_PRIVATE(Core::InputManager*, _inputManager);

    DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::Camera*, _mainCamera);
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::CanvasGroup*, _canvasGroup);
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::UI::Image*, _bgImage);

    DECLARE_INSTANCE_FIELD_PRIVATE(System::Action*, _connectedAction);
    DECLARE_INSTANCE_FIELD_PRIVATE(System::Action_1<GlobalNamespace::DisconnectedReason>*, _disconnectedAction);

    DECLARE_INSTANCE_METHOD(void, Awake);
    DECLARE_INSTANCE_METHOD(void, OnEnable);
    DECLARE_INSTANCE_METHOD(void, OnDisable);
    DECLARE_INSTANCE_METHOD(void, Update);

    DECLARE_INJECT_METHOD(void, Inject, Utilities::SpriteManager* spriteManager, Audio::MicrophoneManager* microphoneManager, GlobalNamespace::IMultiplayerSessionManager* sessionManager, Audio::VoiceManager* voiceManager, Core::InputManager* inputManager);
    DECLARE_CTOR(ctor);
    public:
    private:
        static float get_hudOffsetCamX();
        static float get_hudOffsetCamY();
        static float get_hudOffsetCamZ();
        static UnityEngine::Vector3 get_hudOffset();
        static UnityEngine::Quaternion get_baseRotation();
        static constexpr const float TransitionLerp = 0.1f;

        DisplayState _currentDisplayState = DisplayState::Hidden;
        DisplayState _targetDisplayState = DisplayState::Hidden;

        void RefreshStatus();
        void RefreshStatus_Disconnected(GlobalNamespace::DisconnectedReason reason);
)
