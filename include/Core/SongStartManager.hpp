#pragma once

#include "Core/InputManager.hpp"
#include "custom-types/shared/macros.hpp"

#include "Zenject/IInitializable.hpp"
#include "System/IDisposable.hpp"

#include "Core/InputManager.hpp"
#include "GlobalNamespace/MultiplayerController.hpp"

DECLARE_CLASS_CODEGEN_INTERFACES(MultiplayerChat::Core, SongStartManager, Il2CppObject, std::vector<Il2CppClass*>({ classof(Zenject::IInitializable*), classof(System::IDisposable*)}),
    DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::MultiplayerController*, _multiplayerController);
    DECLARE_INSTANCE_FIELD_PRIVATE(Core::InputManager*, _inputManager);

    DECLARE_INSTANCE_FIELD_PRIVATE(System::Action_1<GlobalNamespace::MultiplayerController::State>*, _stateChangedAction);

    DECLARE_OVERRIDE_METHOD_MATCH(void, Initialize, &::Zenject::IInitializable::Initialize);
    DECLARE_OVERRIDE_METHOD_MATCH(void, Dispose, &::System::IDisposable::Dispose);
    DECLARE_INSTANCE_METHOD(void, OnStateChanged, GlobalNamespace::MultiplayerController::State state);

    DECLARE_CTOR(ctor, GlobalNamespace::MultiplayerController* multiplayerController, Core::InputManager* inputManager);
)