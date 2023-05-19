#pragma once

#include <string>
#include "GlobalNamespace/IConnectedPlayer.hpp"
#include "UnityEngine/Sprite.hpp"

namespace MultiplayerChat::Network {
    class MpcTextChatPacket;
}

namespace MultiplayerChat::Models {
    enum class ChatMessageType {
        PlayerMessage,
        SystemMessage
    };

    struct ChatMessage {
        private:
            ChatMessageType type;
            std::string userId;
            std::string userName;
            std::string text;

            bool senderIsHost;
            bool senderIsMe;
        public:
            auto get_type() const { return type; }
            auto get_userId() const { return userId; }
            auto get_userName() const { return userName; }
            auto get_text() const { return text; }
            auto get_senderIsHost() const { return senderIsHost; }
            auto get_senderIsMe() const { return senderIsMe; }

        ChatMessage(ChatMessageType type, const std::string& userId, const std::string& userName, const std::string& text, bool senderIsHost, bool senderIsMe, bool stripTags = true) :
            type(type),
            userId(userId),
            userName(stripTags ? StripTags(userName) : userName),
            text(stripTags ? StripTags(text) : text),
            senderIsHost(senderIsHost),
            senderIsMe(senderIsMe) {}

        UnityEngine::Sprite* SpriteForMessage(bool inPlayerBubble = false) const;
        std::string FormatMessage(bool inPlayerBubble = false, bool extraIconSpacing = false) const;

        static std::string StripTags(const std::string& input);
        static ChatMessage CreateForLocalPlayer(GlobalNamespace::IConnectedPlayer* player, std::string text);
        static ChatMessage CreateFromPacket(GlobalNamespace::IConnectedPlayer* sender, Network::MpcTextChatPacket* packet);
        static ChatMessage CreateSystemMessage(std::string text);
    };
}
