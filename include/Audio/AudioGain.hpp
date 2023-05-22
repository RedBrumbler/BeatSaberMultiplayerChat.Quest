#pragma once

#include "beatsaber-hook/shared/utils/typedefs.h"
#include <vector>

namespace MultiplayerChat::Audio {
    struct AudioGain {
        static inline void Apply(float* begin, float* end, float gain) {
            if ((gain - 1.0f) < 0.001f) return;
            for (auto itr = begin; itr < end; itr++) *itr = *itr * gain;
        }
        static inline void Apply(ArrayW<float> samples, float gain) {
            if ((gain - 1.0f) < 0.001f) return;
            for (auto& sample : samples) sample *= gain;
        }
        static inline void Apply(std::vector<float> samples, float gain) {
            if ((gain - 1.0f) < 0.001f) return;
            for (auto& sample : samples) sample *= gain;
        }
    };
}
