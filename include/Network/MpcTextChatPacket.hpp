#pragma once

#include "custom-types/shared/macros.hpp"
#include "MpcBasePacket.hpp"

DECLARE_CLASS_CUSTOM(MultiplayerChat::Network, MpcTextChatPacket, MpcBasePacket,
    DECLARE_INSTANCE_FIELD(StringW, text);

    DECLARE_OVERRIDE_METHOD(void, Serialize, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::LiteNetLib::Utils::INetSerializable::Serialize>::get(), LiteNetLib::Utils::NetDataWriter* writer);
    DECLARE_OVERRIDE_METHOD(void, Deserialize, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::LiteNetLib::Utils::INetSerializable::Deserialize>::get(), LiteNetLib::Utils::NetDataReader* reader);

    DECLARE_CTOR(ctor);
)
