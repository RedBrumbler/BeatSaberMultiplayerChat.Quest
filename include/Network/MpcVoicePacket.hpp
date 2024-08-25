#pragma once

#include "custom-types/shared/macros.hpp"
#include "MpcBasePacket.hpp"

#include "GlobalNamespace/IPoolablePacket.hpp"
#include "GlobalNamespace/PacketPool_1.hpp"
#include "Zenject/ArrayPool_1.hpp"
#include "Pooling/ArrayPool.hpp"

DECLARE_CLASS_CUSTOM_INTERFACES(MultiplayerChat::Network, MpcVoicePacket, MpcBasePacket, std::vector<Il2CppClass*>({classof(GlobalNamespace::IPoolablePacket*)}),
    DECLARE_INSTANCE_FIELD(ArrayW<uint8_t>, data);

    DECLARE_OVERRIDE_METHOD_MATCH(void, Serialize, &::LiteNetLib::Utils::INetSerializable::Serialize, LiteNetLib::Utils::NetDataWriter* writer);
    DECLARE_OVERRIDE_METHOD_MATCH(void, Deserialize, &::LiteNetLib::Utils::INetSerializable::Deserialize, LiteNetLib::Utils::NetDataReader* reader);
    DECLARE_OVERRIDE_METHOD_MATCH(void, Release, &::GlobalNamespace::IPoolablePacket::Release);

    DECLARE_CTOR(ctor);

    public:
        std::size_t get_dataLength() const;

        static MpcVoicePacket* Obtain();

        void AllocatePooledBuffer(std::size_t encodedSize);
        void ReturnPooledBuffer();

        GlobalNamespace::IPoolablePacket* i_IPoolablePacket() { return reinterpret_cast<GlobalNamespace::IPoolablePacket*>(this); }
    private:
        static GlobalNamespace::PacketPool_1<MpcVoicePacket*>* get_pool();
        static Pooling::ArrayPool<uint8_t>* get_bytePool();

        bool isRentedBuffer = false;
        std::optional<int> bufferContentSize = std::nullopt;
)
