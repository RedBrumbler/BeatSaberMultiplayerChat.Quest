#include "UI/ModSettings/ModSettingsFlowCoordinator.hpp"

DEFINE_TYPE(MultiplayerChat::UI::ModSettings, ModSettingsFlowCoordinator);

namespace MultiplayerChat::UI::ModSettings {
    void ModSettingsFlowCoordinator::Inject(ModSettingsViewController* viewController, Core::InputManager* inputManager) {
        _viewController = viewController;
        _inputManager = inputManager;
    }

    void ModSettingsFlowCoordinator::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
        _inputManager->set_testMode(true);
        _inputManager->SetActive(true);

        if (!firstActivation) return;

        SetTitle("Multiplayer Chat", HMUI::ViewController::AnimationType::In);
        set_showBackButton(true);
        ProvideInitialViewControllers(_viewController, nullptr, nullptr, nullptr, nullptr);
    }

    void ModSettingsFlowCoordinator::DidDeactivate(bool removedFromHierarchy, bool screenSystemDisabling) {
        _inputManager->set_testMode(false);
        _inputManager->SetActive(false);
    }

    void ModSettingsFlowCoordinator::BackButtonWasPressed(HMUI::ViewController* topViewController) {
        parentFlowCoordinator->DismissFlowCoordinator(this, HMUI::ViewController::AnimationDirection::Vertical, nullptr, false);
    }
}
