#pragma once

#include "custom-types/shared/macros.hpp"
#include "lapiz/shared/macros.hpp"

#include "UnityEngine/MonoBehaviour.hpp"
#include "Zenject/IInitializable.hpp"
#include "System/IDisposable.hpp"

#include "Core/ChatManager.hpp"
#include "Utilities/SpriteManager.hpp"
#include "Models/QuickChatOptions.hpp"

#include "bsml/shared/BSML/Components/ClickableImage.hpp"
#include "bsml/shared/BSML/Components/ModalView.hpp"
#include "TMPro/TextMeshProUGUI.hpp"

DECLARE_CLASS_CODEGEN_INTERFACES(MultiplayerChat::UI::Lobby, QuickChatModal, UnityEngine::MonoBehaviour, std::vector<Il2CppClass*>({classof(Zenject::IInitializable*), classof(System::IDisposable*)}),
    DECLARE_INSTANCE_FIELD_PRIVATE(Models::QuickChatOptions*, _options);
    DECLARE_INSTANCE_FIELD_PRIVATE(MultiplayerChat::Core::ChatManager*, _chatManager);
    DECLARE_INSTANCE_FIELD_PRIVATE(Utilities::SpriteManager*, _spriteManager);

    DECLARE_INSTANCE_FIELD_PRIVATE(BSML::ModalView*, modal);
    DECLARE_INSTANCE_FIELD_PRIVATE(BSML::ClickableImage*, backButton);
    DECLARE_INSTANCE_FIELD_PRIVATE(ArrayW<TMPro::TextMeshProUGUI*>, _buttonTexts);

    DECLARE_OVERRIDE_METHOD_MATCH(void, Initialize, &::Zenject::IInitializable::Initialize);
    DECLARE_OVERRIDE_METHOD_MATCH(void, Dispose, &::System::IDisposable::Dispose);
    DECLARE_INSTANCE_METHOD(void, PostParse);
    DECLARE_INSTANCE_METHOD(void, BackButtonClick);

    DECLARE_INJECT_METHOD(void, Inject, Core::ChatManager* chatManager, Models::QuickChatOptions* options, Utilities::SpriteManager* spriteManager);
    DECLARE_CTOR(ctor);

    public:
        void Show();
        void Hide(bool immediately = false);
    private:
        static const inline std::string RootMenu = "Root";
        std::string _currentMenu = RootMenu;
        bool get_inSubMenu() const { return _currentMenu != RootMenu; }
        const std::vector<std::string>& get_buttonTexts() const {
            auto& opt = _options->get_options();
            auto itr = opt.find(_currentMenu);

            static std::vector<std::string> empty{};
            if (itr == opt.end()) return empty;
            return itr->second;
        }

        ArrayW<TMPro::TextMeshProUGUI*> CreateButtonWheel(UnityEngine::Transform* parent);
        void UpdateButtonTexts();

        void BlockerClicked();
        void IndexedButtonClicked(int idx);
)
