#pragma once

#include <stdint.h>

namespace MultiplayerChat::Utilities {
    enum class InputDeviceCharacteristics : uint32_t {
        None = 0x0u,
        HeadMounted = 0x1u,
        Camera = 0x2u,
        HeldInHand = 0x4u,
        HandTracking = 0x8u,
        EyeTracking = 0x10u,
        TrackedDevice = 0x20u,
        Controller = 0x40u,
        TrackingReference = 0x80u,
        Left = 0x100u,
        Right = 0x200u,
        Simulated6DOF = 0x400u
    };

    static inline InputDeviceCharacteristics operator &(InputDeviceCharacteristics first, InputDeviceCharacteristics second) {
        return static_cast<InputDeviceCharacteristics>(static_cast<uint32_t>(first) & static_cast<uint32_t>(second));
    }

    static inline InputDeviceCharacteristics operator |(InputDeviceCharacteristics first, InputDeviceCharacteristics second) {
        return static_cast<InputDeviceCharacteristics>(static_cast<uint32_t>(first) | static_cast<uint32_t>(second));
    }
}
