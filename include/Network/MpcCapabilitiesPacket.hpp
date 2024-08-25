#pragma once

#include "custom-types/shared/macros.hpp"
#include "MpcBasePacket.hpp"

DECLARE_CLASS_CUSTOM(MultiplayerChat::Network, MpcCapabilitiesPacket, MpcBasePacket,
    DECLARE_INSTANCE_FIELD(bool, canTextChat);
    DECLARE_INSTANCE_FIELD(bool, canReceiveVoiceChat);
    DECLARE_INSTANCE_FIELD(bool, canTransmitVoiceChat);

    DECLARE_OVERRIDE_METHOD_MATCH(void, Serialize, &::LiteNetLib::Utils::INetSerializable::Serialize, LiteNetLib::Utils::NetDataWriter* writer);
    DECLARE_OVERRIDE_METHOD_MATCH(void, Deserialize, &::LiteNetLib::Utils::INetSerializable::Deserialize, LiteNetLib::Utils::NetDataReader* reader);

    DECLARE_CTOR(ctor);
)
