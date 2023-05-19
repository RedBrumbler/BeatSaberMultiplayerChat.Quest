#include "Network/MpcTextChatPacket.hpp"

DEFINE_TYPE(MultiplayerChat::Network, MpcTextChatPacket);

namespace MultiplayerChat::Network {
    void MpcTextChatPacket::ctor() {
        INVOKE_CTOR();
        INVOKE_BASE_CTOR(classof(MpcBasePacket*));
    }

    void MpcTextChatPacket::Serialize(LiteNetLib::Utils::NetDataWriter* writer) {
        MpcBasePacket::Serialize(writer);

        writer->Put(text);
    }

    void MpcTextChatPacket::Deserialize(LiteNetLib::Utils::NetDataReader* reader) {
        MpcBasePacket::Deserialize(reader);

        text = reader->GetString();
    }
}
