#include "Core/SongStartManager.hpp"
#include "custom-types/shared/delegate.hpp"
#include "System/Action_1.hpp"
#include "logging.hpp"
#include <exception>
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

        if (_multiplayerController) _multiplayerController->add_stateChangedEvent(_stateChangedAction);
        else ERROR("MultiplayerController instance is null, cannot add Event");
    }

    void SongStartManager::Dispose() {
        // TODO: Causes crash if just called directly, investigate and optimize this part
        try {
            if (_multiplayerController && _stateChangedAction) {
                _multiplayerController->remove_stateChangedEvent(_stateChangedAction);
                _stateChangedAction = nullptr;
            }
        }
        catch(const std::exception e)
        {
            DEBUG("Exception caught: {}", e.what());
        }
    }

    void SongStartManager::OnStateChanged(GlobalNamespace::MultiplayerController::State state) {
        if (_inputManager) _inputManager->_isSongPlaying = state == GlobalNamespace::MultiplayerController::State::Gameplay; 
    }
}