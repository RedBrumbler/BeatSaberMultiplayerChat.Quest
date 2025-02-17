#include "Core/SongStartManager.hpp"
#include "custom-types/shared/delegate.hpp"
#include "System/Action_1.hpp"
#include <functional>

DEFINE_TYPE(MultiplayerChat::Core, SongStartManager);

namespace MultiplayerChat::Core {
    void SongStartManager::ctor(GlobalNamespace::MultiplayerController* multiplayerController, Core::InputManager* inputManager) {
        INVOKE_CTOR();
        _multiplayerController = multiplayerController;
        _inputManager = inputManager;
    }

    void SongStartManager::Initialize() {

        _stateChangedAction = custom_types::MakeDelegate<System::Action_1<GlobalNamespace::MultiplayerController::State>*>(
            std::function<void(GlobalNamespace::MultiplayerController::State)>(
                std::bind(&SongStartManager::OnStateChanged, this, std::placeholders::_1)
            )
        );

        _multiplayerController->add_stateChangedEvent(_stateChangedAction);
    }

    void SongStartManager::Dispose() {
        _multiplayerController->remove_stateChangedEvent(_stateChangedAction);
    }

    void SongStartManager::OnStateChanged(GlobalNamespace::MultiplayerController::State state) {
        if (state == GlobalNamespace::MultiplayerController::State::Gameplay) {
            _inputManager->set_enabled(true);
        } else {
            _inputManager->set_enabled(false);
        }
    }
}