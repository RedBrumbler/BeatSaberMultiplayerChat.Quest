#pragma once

#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"
#include "UnityEngine/AudioClip.hpp"
#include "UnityEngine/Android/Permission.hpp"
#include "logging.hpp"

namespace MultiplayerChat::Audio {
    class Microphone {
        public:
            static int GetMicrophoneDeviceIDFromName(StringW name) {
                using GetMicrophoneDeviceIDFromName_fun = function_ptr_t<int, StringW>;
                static auto getMicrophoneDeviceIDFromName = reinterpret_cast<GetMicrophoneDeviceIDFromName_fun>(il2cpp_functions::resolve_icall("UnityEngine.Microphone::GetMicrophoneDeviceIDFromName"));
                DEBUG("Getting device ID from name: {}, function ptr", name, fmt::ptr(getMicrophoneDeviceIDFromName));
                return getMicrophoneDeviceIDFromName(name);
            }

            static UnityEngine::AudioClip* StartRecord(int deviceID, bool loop, float lengthSec, int frequency) {
                using StartRecord_fun = function_ptr_t<UnityEngine::AudioClip*, int, bool, float, int>;
                static auto startRecord = reinterpret_cast<StartRecord_fun>(il2cpp_functions::resolve_icall("UnityEngine.Microphone::StartRecord"));
                DEBUG("Starting record: {}, {}, {}, {}, function ptr", deviceID, loop, lengthSec, frequency, fmt::ptr(startRecord));
                return startRecord(deviceID, loop, lengthSec, frequency);
            }

            static void EndRecord(int deviceID) {
                using EndRecord_fun = function_ptr_t<void, int>;
                static auto endRecord = reinterpret_cast<EndRecord_fun>(il2cpp_functions::resolve_icall("UnityEngine.Microphone::EndRecord"));
                DEBUG("Ending record: {}, function ptr", deviceID, fmt::ptr(endRecord));
                endRecord(deviceID);
            }

            static bool IsRecording(int deviceID) {
                using IsRecording_fun = function_ptr_t<bool, int>;
                static auto isRecording = reinterpret_cast<IsRecording_fun>(il2cpp_functions::resolve_icall("UnityEngine.Microphone::IsRecording"));
                DEBUG("Checking if recording: {}, function ptr", deviceID, fmt::ptr(isRecording));
                return isRecording(deviceID);
            }

            static int GetRecordPosition(int deviceID) {
                using GetRecordPosition_fun = function_ptr_t<int, int>;
                static auto getRecordPosition = reinterpret_cast<GetRecordPosition_fun>(il2cpp_functions::resolve_icall("UnityEngine.Microphone::GetRecordPosition"));
                DEBUG("Getting record position: {}, function ptr", deviceID, fmt::ptr(getRecordPosition));
                return getRecordPosition(deviceID);
            }

            static void GetDeviceCaps(int deviceID, int& minFreq, int& maxFreq) {
                using GetDeviceCaps_fun = function_ptr_t<void, int, int*, int*>;
                static auto getDeviceCaps = reinterpret_cast<GetDeviceCaps_fun>(il2cpp_functions::resolve_icall("UnityEngine.Microphone::GetDeviceCaps"));
                getDeviceCaps(deviceID, &minFreq, &maxFreq);
            }

            static ArrayW<StringW> get_devices() {
                using GetDevices_fun = function_ptr_t<ArrayW<StringW>>;
                static auto getDevices = reinterpret_cast<GetDevices_fun>(il2cpp_functions::resolve_icall("UnityEngine.Microphone::get_devices"));
                DEBUG("Getting devices, function ptr", fmt::ptr(getDevices));
                return getDevices();
            }

            static bool HasUserAuthorizedPerrmission(StringW permission) {
                // using UserGrantedPermssion_fun = function_ptr_t<bool, StringW>;
                // static auto hasUserAuthorizedPerrmission = reinterpret_cast<UserGrantedPermssion_fun>(il2cpp_functions::resolve_icall("UnityEngine.Android.Permission::HasUserAuthorizedPermission"));
                // DEBUG("Checking permission: {}, function ptr", permission, fmt::ptr(hasUserAuthorizedPerrmission));
                // return hasUserAuthorizedPerrmission(permission);
                DEBUG("Checking permission: {}", permission);
                return UnityEngine::Android::Permission::HasUserAuthorizedPermission(permission);
            }

            static UnityEngine::AudioClip* Start(StringW deviceName, bool loop, int lengthSec, int frequency);
            static void End(StringW deviceName);
            static bool IsRecording(StringW deviceName);
            static int GetPosition(StringW deviceName);
            static void GetDeviceCaps(StringW deviceName, int& minFreq, int& maxFreq);
    };
}
