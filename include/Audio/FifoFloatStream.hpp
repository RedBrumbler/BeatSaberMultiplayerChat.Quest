#pragma once

#include "custom-types/shared/macros.hpp"
#include <mutex>
#include <stack>
#include <vector>

namespace MultiplayerChat::Audio {
    struct FifoFloatStream {
        public:
            int get_Length() { std::lock_guard<std::mutex> lock(selfmutex); return size; }
            void Close() { Flush(); }
            void Flush();

            int Read(ArrayW<float> buf, int ofs, int count);
            void Write(ArrayW<float> buf, int ofs, int count);

            int Advance(int count);
            int Peek(ArrayW<float> buf, int ofs, int count);

        private:
            float* AllocBlock();
            void FreeBlock(float* block);
            float* GetWBlock();

            static const int BlockSize = 16384;
            static const int MaxBlocksInCache = (3 * 1024 * 1024) / BlockSize;

            std::mutex selfmutex;
            int size;
            int rPos;
            int wPos;

            float* NewBlock() { return new float[BlockSize]; }

            std::stack<float*> usedBlocks;
            std::vector<float*> blocks;
    };
}
