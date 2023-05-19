#pragma once

#include "custom-types/shared/macros.hpp"
#include "MpcBasePacket.hpp"

#include "GlobalNamespace/IPoolablePacket.hpp"
#include "GlobalNamespace/PacketPool_1.hpp"
#include "Zenject/ArrayPool_1.hpp"

DECLARE_CLASS_CUSTOM_INTERFACES(MultiplayerChat::Network, MpcVoicePacket, MpcBasePacket, std::vector<Il2CppClass*>({classof(GlobalNamespace::IPoolablePacket*)}),
    DECLARE_INSTANCE_FIELD(ArrayW<uint8_t>, data);

    DECLARE_OVERRIDE_METHOD(void, Serialize, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::LiteNetLib::Utils::INetSerializable::Serialize>::get(), LiteNetLib::Utils::NetDataWriter* writer);
    DECLARE_OVERRIDE_METHOD(void, Deserialize, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::LiteNetLib::Utils::INetSerializable::Deserialize>::get(), LiteNetLib::Utils::NetDataReader* reader);
    DECLARE_OVERRIDE_METHOD(void, Release, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::GlobalNamespace::IPoolablePacket::Release>::get());

    DECLARE_CTOR(ctor);

    public:
        std::size_t get_dataLength() const;

        static MpcVoicePacket* Obtain();

        void AllocatePooledBuffer(std::size_t encodedSize);
        void ReturnPooledBuffer();
    private:
        static GlobalNamespace::PacketPool_1<MpcVoicePacket*>* get_pool();
        static Zenject::ArrayPool_1<uint8_t>* get_bytePool();

        bool isRentedBuffer = false;
        std::optional<int> bufferContentSize = std::nullopt;
)
