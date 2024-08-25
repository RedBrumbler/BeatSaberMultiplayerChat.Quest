#pragma once

#include "custom-types/shared/macros.hpp"
#include "lapiz/shared/macros.hpp"

#include "HMUI/FlowCoordinator.hpp"
#include "HMUI/ViewController.hpp"
// #include "HMUI/ViewController_AnimationType.hpp"
// #include "HMUI/ViewController_AnimationDirection.hpp"

#include "ModSettingsViewController.hpp"
#include "Core/InputManager.hpp"

DECLARE_CLASS_CODEGEN(MultiplayerChat::UI::ModSettings, ModSettingsFlowCoordinator, HMUI::FlowCoordinator,
    DECLARE_INSTANCE_FIELD_PRIVATE(ModSettingsViewController*, _viewController);
    DECLARE_INSTANCE_FIELD_PRIVATE(Core::InputManager*, _inputManager);

    DECLARE_OVERRIDE_METHOD_MATCH(void, DidActivate, &::HMUI::FlowCoordinator::DidActivate, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);
    DECLARE_OVERRIDE_METHOD_MATCH(void, DidDeactivate, &::HMUI::FlowCoordinator::DidDeactivate, bool removedFromHierarchy, bool screenSystemDisabling);
    DECLARE_OVERRIDE_METHOD_MATCH(void, BackButtonWasPressed, &::HMUI::FlowCoordinator::BackButtonWasPressed, HMUI::ViewController* topViewController);
    DECLARE_INJECT_METHOD(void, Inject, ModSettingsViewController* viewController, Core::InputManager* inputManager);
)
