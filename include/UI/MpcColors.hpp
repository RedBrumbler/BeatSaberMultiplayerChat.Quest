#pragma once

#include "UnityEngine/Color.hpp"

namespace MultiplayerChat::UI {
    struct MpcColors {
        static inline UnityEngine::Color MakeUnityColor(int r, int g, int b, float alpha = 1.0f) {
            return {
                static_cast<float>(r) / 255.0f,
                static_cast<float>(g) / 255.0f,
                static_cast<float>(b) / 255.0f,
                alpha
            };
        }

        static bool IsColorVeryCloseToColor(UnityEngine::Color color0, UnityEngine::Color color1) {
            return  std::abs(color0.r - color1.r) < 0.002f &&
                    std::abs(color0.g - color1.g) < 0.002f &&
                    std::abs(color0.b - color1.b) < 0.002f &&
                    std::abs(color0.a - color1.a) < 0.002f;
        }


        static inline const UnityEngine::Color Green = MakeUnityColor(46, 204, 113, 1.0f);
        static inline const UnityEngine::Color Gold = MakeUnityColor(241, 196, 15, 1.0f);
        static inline const UnityEngine::Color Red = MakeUnityColor(231, 76, 60, 1.0f);
        static inline const UnityEngine::Color Blue = MakeUnityColor(52, 152, 219, 1.0f);
    };
}
