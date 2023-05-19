#pragma once

#include "custom-types/shared/macros.hpp"

#include "Zenject/IInitializable.hpp"
#include "System/IDisposable.hpp"

#include "System/Action.hpp"
#include "Audio/VoiceManager.hpp"
#include "GlobalNamespace/IMultiplayerSessionManager.hpp"
#include "GlobalNamespace/MultiplayerPlayersManager.hpp"

DECLARE_CLASS_CODEGEN_INTERFACES(MultiplayerChat::Core, GameplayIntegrator, Il2CppObject, std::vector<Il2CppClass*>({ classof(Zenject::IInitializable*), classof(System::IDisposable*)}),
    DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::IMultiplayerSessionManager*, _sessionManager);
    DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::MultiplayerPlayersManager*, _playersManager);
    DECLARE_INSTANCE_FIELD_PRIVATE(Audio::VoiceManager*, _voiceManager);

    DECLARE_INSTANCE_FIELD_PRIVATE(System::Action*, _playersSpawnedAction);

    DECLARE_OVERRIDE_METHOD(void, Initialize, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::Zenject::IInitializable::Initialize>::get());
    DECLARE_OVERRIDE_METHOD(void, Dispose, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::System::IDisposable::Dispose>::get());

    DECLARE_CTOR(ctor, GlobalNamespace::IMultiplayerSessionManager* sessionManager, GlobalNamespace::MultiplayerPlayersManager* playersManager, Audio::VoiceManager* voiceManager);

    private:
        void HandlePlayersSpawned();
)
