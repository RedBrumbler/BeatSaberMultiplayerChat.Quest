#include "hooking.hpp"
#include "logging.hpp"

#include "Core/LobbyIntegrator.hpp"

#include "GlobalNamespace/GameServerPlayerTableCell.hpp"
#include "GlobalNamespace/MultiplayerLobbyAvatarManager.hpp"
#include "GlobalNamespace/LobbySetupViewController.hpp"
#include "HMUI/ViewController.hpp"
#include "GlobalNamespace/GameServerLobbyFlowCoordinator.hpp"

MAKE_AUTO_HOOK_MATCH(
    GameServerPlayerTableCell_SetData,
    &::GlobalNamespace::GameServerPlayerTableCell::SetData,
    void,
    GlobalNamespace::GameServerPlayerTableCell* self,
    ::GlobalNamespace::IConnectedPlayer* connectedPlayer,
    ::GlobalNamespace::ILobbyPlayerData* playerData,
    bool hasKickPermissions,
    bool allowSelection,
    ::System::Threading::Tasks::Task_1<::GlobalNamespace::EntitlementStatus>* getLevelEntitlementTask
) {
    GameServerPlayerTableCell_SetData(
        self,
        connectedPlayer,
        playerData,
        hasKickPermissions,
        allowSelection,
        getLevelEntitlementTask
    );

    auto integrator = MultiplayerChat::Core::LobbyIntegrator::get_instance();
    if (integrator)
        integrator->PostfixPlayerCellSetData(connectedPlayer, self->_mutePlayerButton);
}

MAKE_AUTO_HOOK_MATCH(
    MultiplayerLobbyAvatarManager_AddPlayer,
    &::GlobalNamespace::MultiplayerLobbyAvatarManager::AddPlayer,
    void,
    GlobalNamespace::MultiplayerLobbyAvatarManager* self,
    ::GlobalNamespace::IConnectedPlayer* connectedPlayer
) {
    MultiplayerLobbyAvatarManager_AddPlayer(self, connectedPlayer);
    auto integrator = MultiplayerChat::Core::LobbyIntegrator::get_instance();
    if (integrator)
        integrator->PostFixLobbyAvatarAddPlayer(connectedPlayer, self->_playerIdToAvatarMap);
}

MAKE_AUTO_HOOK_MATCH(
    LobbySetupViewController_DidActivate,
    &::GlobalNamespace::LobbySetupViewController::DidActivate,
    void,
    GlobalNamespace::LobbySetupViewController* self,
    bool firstActivation,
    bool addedToHierarchy,
    bool screenSystemEnabling
) {
    LobbySetupViewController_DidActivate(self, firstActivation, addedToHierarchy, screenSystemEnabling);
    auto integrator = MultiplayerChat::Core::LobbyIntegrator::get_instance();
    if (integrator)
        integrator->PostfixLobbySetupActivation();
}

MAKE_AUTO_HOOK_MATCH(
    ViewController_Deactivate,
    &::HMUI::ViewController::__Deactivate,
    void,
    HMUI::ViewController* self,
    bool removedFromHierarchy,
    bool deactivateGameObject,
    bool screenSystemDisabling
) {
    ViewController_Deactivate(self, removedFromHierarchy, deactivateGameObject, screenSystemDisabling);
    auto integrator = MultiplayerChat::Core::LobbyIntegrator::get_instance();
    if (integrator)
        integrator->PostfixLobbySetupDeactivation();
}

MAKE_AUTO_HOOK_ORIG_MATCH(
    GameServerLobbyFlowCoordinator_SetTitle,
    &::GlobalNamespace::GameServerLobbyFlowCoordinator::SetTitle,
    void,
    GlobalNamespace::GameServerLobbyFlowCoordinator* self,
    HMUI::ViewController *newViewController,
    HMUI::ViewController::AnimationType animationType
) {
    auto integrator = MultiplayerChat::Core::LobbyIntegrator::get_instance();
    if (integrator) {
        if (integrator->PrefixFlowCoordinatorSetTitle(newViewController, animationType))
            GameServerLobbyFlowCoordinator_SetTitle(self, newViewController, animationType);
    } else {
        GameServerLobbyFlowCoordinator_SetTitle(self, newViewController, animationType);
    }
}
