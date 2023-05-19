#pragma once

#include "GlobalNamespace/IConnectedPlayer.hpp"
#include "Network/MpcCapabilitiesPacket.hpp"

namespace MultiplayerChat::Models {
    struct ChatPlayer {
        GlobalNamespace::IConnectedPlayer* const player;

        bool canTextChat;
        bool canReceiveVoiceChat;
        bool canTransmitVoiceChat;

        std::string get_userId() const { return player->get_userId(); }
        std::string get_userName() const { return player->get_userName(); }
        bool get_isMe() const { return player->get_isMe(); }

        bool isTyping = false;
        bool isSpeaking = false;
        bool isMuted = false;

        ChatPlayer(GlobalNamespace::IConnectedPlayer* player, Network::MpcCapabilitiesPacket* capabilities)
        : player(player), canTextChat(capabilities->canTextChat), canReceiveVoiceChat(capabilities->canReceiveVoiceChat), canTransmitVoiceChat(capabilities->canTransmitVoiceChat) {}
    };
}
