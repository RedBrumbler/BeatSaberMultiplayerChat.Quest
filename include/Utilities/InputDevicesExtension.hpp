#pragma once

#include "UnityEngine/XR/XRNode.hpp"
#include "UnityEngine/XR/InputDevice.hpp"
#include "UnityEngine/XR/InputFeatureUsage.hpp"
#include "InputDeviceCharacteristics.hpp"

namespace MultiplayerChat::Utilities {


    struct InputDevicesExtension {
        static bool IsDeviceValid(UnityEngine::XR::InputDevice device);
        static bool IsDeviceValid(uint64_t deviceId);

        static bool TryGetFeatureValue(UnityEngine::XR::InputDevice& device, UnityEngine::XR::InputFeatureUsage usage, float& value);
        static bool TryGetFeatureValue(UnityEngine::XR::InputDevice& device, UnityEngine::XR::InputFeatureUsage usage, bool& value);

        static StringW GetDeviceName(UnityEngine::XR::InputDevice device);
        static StringW GetDeviceName(uint64_t deviceId);

        static UnityEngine::XR::InputDevice GetDeviceAtXRNode(UnityEngine::XR::XRNode node);
        static uint64_t GetDeviceIdAtXRNode(UnityEngine::XR::XRNode node);

        static InputDeviceCharacteristics GetDeviceCharacteristics(UnityEngine::XR::InputDevice device);
        static InputDeviceCharacteristics GetDeviceCharacteristics(uint64_t deviceId);
    };
}
