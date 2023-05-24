#include "Models/ChatMessage.hpp"
#include <fmt/core.h>
#include <regex>

#include "Network/MpcTextChatPacket.hpp"
#include "bsml/shared/Helpers/utilities.hpp"

namespace MultiplayerChat::Models {
    std::string ChatMessage::FormatMessage(bool inPlayerBubble, bool extraIconSpacing) const {
        auto spacing = extraIconSpacing ? "\t" : "";
        if (inPlayerBubble)
            return fmt::format("{}<i>{}</i>", spacing, text);
        else if (type == ChatMessageType::SystemMessage)
            return fmt::format("{}<i><color=#f1c40f>[System]</color> <color=#ecf0f1>{}</color></i>", spacing, text);
        else if (senderIsHost)
            return fmt::format("{}<i><color=#2ecc71>[Server]</color> {}</i>", spacing, text);
        else if (senderIsMe)
            return fmt::format("{}<i><color=#95a5a6>[{}]</color> {}</i>", spacing, userName, text);
        else
            return fmt::format("{}<i><color=#3498db>[{}]</color> {}</i>", spacing, userName, text);
    }

    UnityEngine::Sprite* ChatMessage::SpriteForMessage(bool inPlayerBubble) const {
        if (inPlayerBubble)
            return BSML::Utilities::FindSpriteCached("PlayerIcon");
        else if (type == ChatMessageType::SystemMessage)
            return BSML::Utilities::FindSpriteCached("NoFailIcon");
        else if (senderIsHost)
            return BSML::Utilities::FindSpriteCached("GlobalIcon");
        else if (senderIsMe)
            return BSML::Utilities::FindSpriteCached("PlayerIcon");
        else
            return BSML::Utilities::FindSpriteCached("PlayerIcon");
    }

    std::string ChatMessage::StripTags(const std::string& input) {
        std::regex tags_re("<.*?>");
        return std::regex_replace(input, tags_re, "");
    }

    ChatMessage ChatMessage::CreateForLocalPlayer(GlobalNamespace::IConnectedPlayer* player, std::string text) {
        return ChatMessage(
            ChatMessageType::PlayerMessage,
            player->get_userId(),
            player->get_userName(),
            text,
            player->get_isConnectionOwner(),
            player->get_isMe(),
            true
        );
    }

    ChatMessage ChatMessage::CreateFromPacket(GlobalNamespace::IConnectedPlayer* sender, Network::MpcTextChatPacket* packet) {
        return ChatMessage(
            ChatMessageType::PlayerMessage,
            sender->get_userId(),
            sender->get_userName(),
            packet ? (packet->text ? packet->text : "") : "",
            sender->get_isConnectionOwner(),
            sender->get_isMe(),
            true
        );
    }

    ChatMessage ChatMessage::CreateSystemMessage(std::string text) {
        return ChatMessage(
            ChatMessageType::SystemMessage,
            "system",
            "System",
            text,
            false,
            false,
            false
        );
    }
}
