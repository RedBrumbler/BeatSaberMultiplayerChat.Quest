#include "UI/ModSettings/ModSettingsMenuController.hpp"
#include "bsml/shared/BSML.hpp"

DEFINE_TYPE(MultiplayerChat::UI::ModSettings, ModSettingsMenuController);

namespace MultiplayerChat::UI::ModSettings {
    void ModSettingsMenuController::ctor(ModSettingsFlowCoordinator* modSettingsFlowCoordinator, GlobalNamespace::MainFlowCoordinator* mainFlowCoordinator) {
        INVOKE_CTOR();
        _modSettingsFlowCoordinator = modSettingsFlowCoordinator;
        _mainFlowCoordinator = mainFlowCoordinator;
    }

    void ModSettingsMenuController::Initialize() {
        _menuButton = BSML::Register::RegisterMenuButton(
            "Multiplayer Chat",
            "MAnage and test your Multiplayer Chat settings",
            std::bind(&ModSettingsMenuController::HandleMenuButtonClick, this)
        );
    }

    void ModSettingsMenuController::Dispose() {
        if (_menuButton) {
            BSML::Register::UnRegisterMenuButton(_menuButton);
            _menuButton = nullptr;
        }
    }

    void ModSettingsMenuController::HandleMenuButtonClick() {
        auto fc = _mainFlowCoordinator->YoungestChildFlowCoordinatorOrSelf();
        fc->PresentFlowCoordinator(_modSettingsFlowCoordinator, nullptr, HMUI::ViewController::AnimationDirection::Horizontal, false, false);
    }
}
