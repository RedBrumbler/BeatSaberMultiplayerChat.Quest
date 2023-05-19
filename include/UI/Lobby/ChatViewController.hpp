#pragma once

#include "custom-types/shared/macros.hpp"
#include "lapiz/shared/macros.hpp"

#include "HMUI/ViewController.hpp"
#include "HMUI/ViewController_AnimationType.hpp"
#include "HMUI/ViewController_AnimationDirection.hpp"
#include "TMPro/TextMeshProUGUI.hpp"

#include "bsml/shared/BSML/Components/Backgroundable.hpp"
#include "bsml/shared/BSML/Components/ScrollableContainer.hpp"
#include "bsml/shared/BSML/Components/Settings/StringSetting.hpp"

#include "Core/ChatManager.hpp"
#include "Models/ChatMessage.hpp"
#include <vector>

DECLARE_CLASS_CODEGEN(MultiplayerChat::UI::Lobby, ChatViewController, HMUI::ViewController,
    DECLARE_INSTANCE_FIELD_PRIVATE(Core::ChatManager*, _chatManager);
    DECLARE_INSTANCE_FIELD_PRIVATE(BSML::Backgroundable*, chatViewBg);
    DECLARE_INSTANCE_FIELD_PRIVATE(BSML::ScrollableContainer*, scrollableContainer);
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::Transform*, scrollableContainerContent);
    DECLARE_INSTANCE_FIELD_PRIVATE(BSML::StringSetting*, chatInput);

    DECLARE_INSTANCE_METHOD(void, Update);
    DECLARE_INSTANCE_METHOD(void, PostParse);

    DECLARE_OVERRIDE_METHOD(void, DidActivate, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::HMUI::ViewController::DidActivate>::get(), bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);

    DECLARE_INJECT_METHOD(void, Inject, Core::ChatManager* chatManager);
    DECLARE_CTOR(ctor);
    public:
        void ClearMessages(bool visualOnly = false);
        void AddMessage(const Models::ChatMessage& message, bool visualOnly = false);
    private:
        static inline ConstString ChatInputPlaceholderText{"Click here to type a message"};
        static constexpr const int MaxBufferSize = 32;

        bool _bsmlReady = false;
        bool _chatLockedToBottom = true;
        std::vector<Models::ChatMessage> _messageBuffer;


        void HandleKeyboardInput(StringW input);
        custom_types::Helpers::Coroutine HandleKeyboardInputRoutine(StringW input);

        void ApplyUIMutations();
        void ResetChatInputText();

        void FillChat();
        TMPro::TextMeshProUGUI* AddTextObject();

        void HandleScrollablePageUp();
        void HandleScrollablePageDown();
)
