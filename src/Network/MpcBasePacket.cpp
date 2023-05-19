#include "Network/MpcBasePacket.hpp"
#include "Network/MpcVersionInfo.hpp"

#include "GlobalNamespace/VarIntExtensions.hpp"

DEFINE_TYPE(MultiplayerChat::Network, MpcBasePacket);

namespace MultiplayerChat::Network {
    void MpcBasePacket::ctor() {
        INVOKE_CTOR();
    }

    void MpcBasePacket::Serialize(LiteNetLib::Utils::NetDataWriter* writer) {
        protocolVersion = MpcVersionInfo::ProtocolVersion;

        GlobalNamespace::VarIntExtensions::PutVarUInt(writer, protocolVersion);
    }

    void MpcBasePacket::Deserialize(LiteNetLib::Utils::NetDataReader* reader) {
        protocolVersion = GlobalNamespace::VarIntExtensions::GetVarUInt(reader);
    }
}
