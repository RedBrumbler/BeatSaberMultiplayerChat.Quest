#include "Network/MpcCapabilitiesPacket.hpp"

DEFINE_TYPE(MultiplayerChat::Network, MpcCapabilitiesPacket);

namespace MultiplayerChat::Network {
    void MpcCapabilitiesPacket::ctor() {
        INVOKE_CTOR();
        INVOKE_BASE_CTOR(classof(MpcBasePacket*));
    }

    void MpcCapabilitiesPacket::Serialize(LiteNetLib::Utils::NetDataWriter* writer) {
        MpcBasePacket::Serialize(writer);

        writer->Put(canTextChat);
        writer->Put(canReceiveVoiceChat);
        writer->Put(canTransmitVoiceChat);
    }

    void MpcCapabilitiesPacket::Deserialize(LiteNetLib::Utils::NetDataReader* reader) {
        MpcBasePacket::Deserialize(reader);

        canTextChat = reader->GetBool();
        canReceiveVoiceChat = reader->GetBool();
        canTransmitVoiceChat = reader->GetBool();
    }
}
