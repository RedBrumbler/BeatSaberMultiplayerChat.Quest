#pragma once

#include "custom-types/shared/macros.hpp"
#include "MpcBasePacket.hpp"

DECLARE_CLASS_CUSTOM(MultiplayerChat::Network, MpcCapabilitiesPacket, MpcBasePacket,
    DECLARE_INSTANCE_FIELD(bool, canTextChat);
    DECLARE_INSTANCE_FIELD(bool, canReceiveVoiceChat);
    DECLARE_INSTANCE_FIELD(bool, canTransmitVoiceChat);

    DECLARE_OVERRIDE_METHOD(void, Serialize, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::LiteNetLib::Utils::INetSerializable::Serialize>::get(), LiteNetLib::Utils::NetDataWriter* writer);
    DECLARE_OVERRIDE_METHOD(void, Deserialize, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::LiteNetLib::Utils::INetSerializable::Deserialize>::get(), LiteNetLib::Utils::NetDataReader* reader);

    DECLARE_CTOR(ctor);
)
