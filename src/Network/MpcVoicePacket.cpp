#include "Network/MpcVoicePacket.hpp"
#include "GlobalNamespace/ThreadStaticPacketPool_1.hpp"
#include "Audio/VoiceManager.hpp"

DEFINE_TYPE(MultiplayerChat::Network, MpcVoicePacket);

namespace MultiplayerChat::Network {
    void MpcVoicePacket::ctor() {
        INVOKE_CTOR();
        INVOKE_BASE_CTOR(classof(MpcBasePacket*));
    }

    std::size_t MpcVoicePacket::get_dataLength() const {
        return bufferContentSize.value_or(data ? data.size() : 0);
    }

    void MpcVoicePacket::Serialize(LiteNetLib::Utils::NetDataWriter* writer) {
        MpcBasePacket::Serialize(writer);

        if (!data || bufferContentSize.value_or(0) == 0) {
            writer->Put(0);
            return;
        }

        writer->PutBytesWithLength(data, 0, get_dataLength());
    }

    void MpcVoicePacket::Deserialize(LiteNetLib::Utils::NetDataReader* reader) {
        MpcBasePacket::Deserialize(reader);

        data = reader->GetBytesWithLength();
    }

    GlobalNamespace::PacketPool_1<MpcVoicePacket*>* MpcVoicePacket::get_pool() {
        static SafePtr<GlobalNamespace::PacketPool_1<MpcVoicePacket*>> pool;

        if (!pool || !pool.ptr())
            pool = GlobalNamespace::ThreadStaticPacketPool_1<MpcVoicePacket*>::get_pool();
        return pool.ptr();
    }

    MpcVoicePacket* MpcVoicePacket::Obtain() {
        return get_pool()->Obtain();
    }

    void MpcVoicePacket::Release() {
        ReturnPooledBuffer();
        data = nullptr;

        isRentedBuffer = false;
        bufferContentSize = std::nullopt;

        get_pool()->Release(this);
    }

    Zenject::ArrayPool_1<uint8_t>* MpcVoicePacket::get_bytePool() {
        static SafePtr<Zenject::ArrayPool_1<uint8_t>> pool;
        if (!pool || !pool.ptr())
            pool = Zenject::ArrayPool_1<uint8_t>::GetPool(Audio::VoiceManager::FrameByteSize);
        return pool.ptr();
    }

    void MpcVoicePacket::AllocatePooledBuffer(std::size_t encodedSize) {
        ReturnPooledBuffer();

        data = get_bytePool()->Spawn();

        if (data.size() < encodedSize)
            throw std::runtime_error("This should never happen: rented buffer is too smol");

        isRentedBuffer = true;
        bufferContentSize = encodedSize;
    }

    void MpcVoicePacket::ReturnPooledBuffer() {
        if (!data || !isRentedBuffer) return;
        get_bytePool()->Despawn(data);

        data = nullptr;

        isRentedBuffer = false;
        bufferContentSize = std::nullopt;
    }
}
