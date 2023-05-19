#include "Utilities/SpriteManager.hpp"

#include "assets.hpp"
#include "bsml/shared/Helpers/utilities.hpp"
DEFINE_TYPE(MultiplayerChat::Utilities, SpriteManager);

#define CLEAN_SPRITE(sprite) \
    if (sprite && sprite->m_CachedPtr.m_value)\
        UnityEngine::Object::Destroy(sprite);\
    sprite = nullptr

namespace MultiplayerChat::Utilities {
    void SpriteManager::Initialize() {
        _chatIcon = BSML::Utilities::LoadSpriteRaw(IncludedAssets::Chat_png);
        _keyboardIcon = BSML::Utilities::LoadSpriteRaw(IncludedAssets::Keyboard_png);
        _micOffIcon = BSML::Utilities::LoadSpriteRaw(IncludedAssets::MicOff_png);
        _micOnIcon = BSML::Utilities::LoadSpriteRaw(IncludedAssets::MicOn_png);
        _unreadBadgeIcon = BSML::Utilities::LoadSpriteRaw(IncludedAssets::UnreadBadge_png);
    }

    void SpriteManager::Dispose() {
        CLEAN_SPRITE(_chatIcon);
        CLEAN_SPRITE(_keyboardIcon);
        CLEAN_SPRITE(_micOffIcon);
        CLEAN_SPRITE(_micOnIcon);
        CLEAN_SPRITE(_unreadBadgeIcon);
    }
}
