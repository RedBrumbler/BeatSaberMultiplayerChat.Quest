#pragma once

#include "custom-types/shared/macros.hpp"

#include "Zenject/IInitializable.hpp"
#include "System/IDisposable.hpp"
#include "GlobalNamespace/MainFlowCoordinator.hpp"
#include "ModSettingsFlowCoordinator.hpp"

#include "bsml/shared/BSML/MenuButtons/MenuButton.hpp"

DECLARE_CLASS_CODEGEN_INTERFACES(MultiplayerChat::UI::ModSettings, ModSettingsMenuController, Il2CppObject, std::vector<Il2CppClass*>({ classof(Zenject::IInitializable*), classof(System::IDisposable*)}),
    DECLARE_INSTANCE_FIELD_PRIVATE(BSML::MenuButton*, _menuButton);
    DECLARE_INSTANCE_FIELD_PRIVATE(ModSettingsFlowCoordinator*, _modSettingsFlowCoordinator);
    DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::MainFlowCoordinator*, _mainFlowCoordinator);

    DECLARE_OVERRIDE_METHOD(void, Initialize, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::Zenject::IInitializable::Initialize>::get());
    DECLARE_OVERRIDE_METHOD(void, Dispose, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::System::IDisposable::Dispose>::get());

    DECLARE_CTOR(ctor, ModSettingsFlowCoordinator* modSettingsFlowCoordinator, GlobalNamespace::MainFlowCoordinator* mainFlowCoordinator);
    private:
        void HandleMenuButtonClick();
)
