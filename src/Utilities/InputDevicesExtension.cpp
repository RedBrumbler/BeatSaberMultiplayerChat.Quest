#include "Utilities/InputDevicesExtension.hpp"

namespace MultiplayerChat::Utilities {
    bool InputDevicesExtension::IsDeviceValid(UnityEngine::XR::InputDevice device) {
        return device.m_DeviceId != std::numeric_limits<uint64_t>::max() && IsDeviceValid(device.m_DeviceId);
    }

    bool InputDevicesExtension::IsDeviceValid(uint64_t deviceId) {
        using IsDeviceValid_fun = function_ptr_t<bool, uint64_t>;
        static auto isDeviceValid = reinterpret_cast<IsDeviceValid_fun>(il2cpp_functions::resolve_icall("UnityEngine.XR.InputDevices::IsDeviceValid"));
        return isDeviceValid(deviceId);
    }

    bool InputDevicesExtension::TryGetFeatureValue(UnityEngine::XR::InputDevice& device, UnityEngine::XR::InputFeatureUsage usage, float& value) {
        using TryGetFeatureValue_fun = function_ptr_t<bool, uint64_t, StringW, float*>;
        static auto tryGetFeatureValue = reinterpret_cast<TryGetFeatureValue_fun>(il2cpp_functions::resolve_icall("UnityEngine.XR.InputDevices::TryGetFeatureValue_float"));
        return tryGetFeatureValue(device.get_deviceId(), usage.m_Name, &value);
    }

    bool InputDevicesExtension::TryGetFeatureValue(UnityEngine::XR::InputDevice& device, UnityEngine::XR::InputFeatureUsage usage, bool& value) {
        using TryGetFeatureValue_fun = function_ptr_t<bool, uint64_t, StringW, bool*>;
        static auto tryGetFeatureValue = reinterpret_cast<TryGetFeatureValue_fun>(il2cpp_functions::resolve_icall("UnityEngine.XR.InputDevices::TryGetFeatureValue_bool"));
        return tryGetFeatureValue(device.get_deviceId(), usage.m_Name, &value);
    }

    StringW InputDevicesExtension::GetDeviceName(UnityEngine::XR::InputDevice device) {
        return IsDeviceValid(device) ? GetDeviceName(device.get_deviceId()) : nullptr;
    }

    StringW InputDevicesExtension::GetDeviceName(uint64_t deviceId) {
        using GetDeviceName_fun = function_ptr_t<StringW, uint64_t>;
        static auto getDeviceName = reinterpret_cast<GetDeviceName_fun>(il2cpp_functions::resolve_icall("UnityEngine.XR.InputDevices::GetDeviceName"));
        return getDeviceName(deviceId);
    }

    UnityEngine::XR::InputDevice InputDevicesExtension::GetDeviceAtXRNode(UnityEngine::XR::XRNode node) {
        auto device = UnityEngine::XR::InputDevice();
        il2cpp_utils::RunMethod(device, ".ctor", GetDeviceIdAtXRNode(node));
        return device;
    }

    uint64_t InputDevicesExtension::GetDeviceIdAtXRNode(UnityEngine::XR::XRNode node) {
        using GetDeviceIdAtXRNode_fun = function_ptr_t<uint64_t, UnityEngine::XR::XRNode>;
        static auto getDeviceIdAtXRNode = reinterpret_cast<GetDeviceIdAtXRNode_fun>(il2cpp_functions::resolve_icall("UnityEngine.XR.InputTracking::GetDeviceIdAtXRNode"));
        return getDeviceIdAtXRNode(node);
    }

    InputDeviceCharacteristics InputDevicesExtension::GetDeviceCharacteristics(UnityEngine::XR::InputDevice device) {
        return IsDeviceValid(device) ? GetDeviceCharacteristics(device.get_deviceId()) : InputDeviceCharacteristics::None;
    }

    InputDeviceCharacteristics InputDevicesExtension::GetDeviceCharacteristics(uint64_t deviceId) {
        using GetDeviceCharacteristics_fun = function_ptr_t<InputDeviceCharacteristics, uint64_t>;
        static auto getDeviceCharacteristics = reinterpret_cast<GetDeviceCharacteristics_fun>(il2cpp_functions::resolve_icall("UnityEngine.XR.InputDevices::GetDeviceCharacteristics"));
        return getDeviceCharacteristics(deviceId);
    }
}
