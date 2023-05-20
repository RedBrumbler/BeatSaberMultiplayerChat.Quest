#include "Audio/FifoFloatStream.hpp"

namespace MultiplayerChat::Audio {
    float* FifoFloatStream::AllocBlock() {
        float* result = nullptr;
        if (!usedBlocks.empty()) {
            result = usedBlocks.top();
            usedBlocks.pop();
            return result;
        }
        return NewBlock();
    }

    void FifoFloatStream::FreeBlock(float* block) {
        if (usedBlocks.size() < MaxBlocksInCache)
            usedBlocks.push(block);
        else delete[] block;
    }

    float* FifoFloatStream::GetWBlock() {
        float* result = nullptr;
        if (wPos < BlockSize && blocks.size() > 0)
            result = blocks.back();
        else {
            result = AllocBlock();
            blocks.emplace_back(result);
            wPos = 0;
        }

        return result;
    }

    void FifoFloatStream::Flush() {
        std::lock_guard<std::mutex> lock(selfmutex);

        for (auto block : blocks) FreeBlock(block);
        blocks.clear();
        rPos = 0;
        wPos = 0;
        size = 0;
    }

    int FifoFloatStream::Read(ArrayW<float> buf, int ofs, int count) {
        std::lock_guard<std::mutex> lock(selfmutex);
        auto result = Peek(buf, ofs, count);
        Advance(result);
        return result;
    }

    void FifoFloatStream::Write(ArrayW<float> buf, int ofs, int count) {
        std::lock_guard<std::mutex> lock(selfmutex);
        auto left = count;
        while (left > 0) {
            auto toWrite = std::min(BlockSize - wPos, left);
            auto wBlock = GetWBlock();

            auto destStart = wBlock + wPos;
            auto srcStart = buf.begin() + ofs + count - left;

            memcpy(destStart, srcStart, toWrite * sizeof(float));

            wPos += toWrite;
            left -= toWrite;
        }

        size += count;
    }

    int FifoFloatStream::Advance(int count) {
        std::lock_guard<std::mutex> lock(selfmutex);

        auto sizeLeft = count;
        while (sizeLeft > 0 && size > 0) {
            if (rPos == BlockSize) {
                rPos = 0;
                FreeBlock(blocks.front());
                blocks.erase(blocks.begin());
            }

            auto toFeed = blocks.size() == 1 ? std::min(wPos - rPos, sizeLeft) : std::min(BlockSize - rPos, sizeLeft);

            rPos += toFeed;
            sizeLeft -= toFeed;
            size -= toFeed;
        }

        return count - sizeLeft;
    }

    int FifoFloatStream::Peek(ArrayW<float> buf, int ofs, int count) {
        std::lock_guard<std::mutex> lock(selfmutex);
        auto sizeLeft = count;
        auto tempBlockPos = rPos;
        auto tempSize = size;

        auto currentBlock = 0;
        while (sizeLeft > 0 && tempSize > 0) {
            if (tempBlockPos == BlockSize) {
                tempBlockPos = 0;
                currentBlock++;
            }

            auto upper = (currentBlock < (blocks.size() - 1)) ? BlockSize : wPos;
            auto toFeed = std::min(upper - tempBlockPos, sizeLeft);

            auto destStart = buf.begin() + ofs + count - sizeLeft;
            auto srcStart = blocks.at(currentBlock) + tempBlockPos;

            memcpy(destStart, srcStart, toFeed * sizeof(float));

            sizeLeft -= toFeed;
            tempBlockPos += toFeed;
            tempSize -= toFeed;
        }

        return count - sizeLeft;
    }
}
