#pragma once

#include "beatsaber-hook/shared/utils/typedefs.h"
#include <stdint.h>
#include <stack>
#include "System/Collections/Generic/Stack_1.hpp"

namespace MultiplayerChat::Pooling {
    template<typename T>
    struct ArrayPool {
        using PooledArrayW = ArrayW<T>;
        using PooledArray = Array<T>;

        ArrayPool(std::size_t sz) : sz(sz), _pool(System::Collections::Generic::Stack_1<PooledArrayW>::New_ctor()) {}

        void Despawn(PooledArrayW arr) {
            std::lock_guard<std::mutex> lock(selfmutex);
            _pool->Push(arr);
        }

        PooledArrayW Spawn() {
            std::lock_guard<std::mutex> lock(selfmutex);
            if (_pool->get_Count() == 0) {
                return PooledArrayW(il2cpp_array_size_t(sz));
            } else return _pool->Pop();
        }

        const std::size_t sz;
        private:
            std::mutex selfmutex;
            SafePtr<System::Collections::Generic::Stack_1<PooledArrayW>> _pool;
    };
}
