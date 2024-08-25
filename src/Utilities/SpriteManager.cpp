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
        _chatIcon = BSML::Utilities::LoadSpriteRaw(Assets::Chat_png);
        _chatIcon->set_name("ChatIcon");
        _keyboardIcon = BSML::Utilities::LoadSpriteRaw(Assets::Keyboard_png);
        _keyboardIcon->set_name("KeyboardIcon");
        _micOffIcon = BSML::Utilities::LoadSpriteRaw(Assets::MicOff_png);
        _micOffIcon->set_name("MicOffIcon");
        _micOnIcon = BSML::Utilities::LoadSpriteRaw(Assets::MicOn_png);
        _micOnIcon->set_name("MicOnIcon");
        _unreadBadgeIcon = BSML::Utilities::LoadSpriteRaw(Assets::UnreadBadge_png);
        _unreadBadgeIcon->set_name("UnreadBadgeIcon");
        _bgMask = BSML::Utilities::LoadSpriteRaw(Assets::BG_Mask_png);
        _bgMask->set_name("BG_Mask");
        _radialButton = BSML::Utilities::LoadSpriteRaw(Assets::RadialButton_png);
        _radialButton->set_name("RadialButton");
    }

    void SpriteManager::Dispose() {
        CLEAN_SPRITE(_chatIcon);
        CLEAN_SPRITE(_keyboardIcon);
        CLEAN_SPRITE(_micOffIcon);
        CLEAN_SPRITE(_micOnIcon);
        CLEAN_SPRITE(_unreadBadgeIcon);
        CLEAN_SPRITE(_bgMask);
        CLEAN_SPRITE(_radialButton);
    }
}
