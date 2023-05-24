#include "Audio/MicrophoneManager.hpp"
#include "logging.hpp"
#include "config.hpp"

DEFINE_TYPE(MultiplayerChat::Audio, MicrophoneManager);

namespace MultiplayerChat::Audio {
    void MicrophoneManager::ctor() {
        INVOKE_CTOR();

        _micBufferPos = 0;
        _fragmentBuffer = ArrayW<float>(il2cpp_array_size_t(SamplesPerFragment));
        _micBuffer = nullptr;
    }

    void MicrophoneManager::Initialize() {
        TryAutoSelectDevice();
    }

    void MicrophoneManager::Dispose() {
        if (_captureClip && _captureClip->m_CachedPtr.m_value)
            UnityEngine::Object::Destroy(_captureClip);
        _captureClip = nullptr;
    }

    static inline bool is_null(UnityEngine::Object* obj) {
        return !obj || !obj->m_CachedPtr.m_value;
    }

    static inline bool not_null(UnityEngine::Object* obj) {
        return obj && obj->m_CachedPtr.m_value;
    }

    void MicrophoneManager::Tick() {
        if (!get_isCapturing() || is_null(_captureClip) || !_micBuffer) return;

        auto micPosCurrent = Microphone::GetPosition(get_selectedDeviceName());

        if (micPosCurrent < 0 || _micBufferPos == micPosCurrent) return;

        if (!_captureClip->GetData(_micBuffer, 0))
            return;

        while(GetLoopDataLength(_micBuffer.size(), _micBufferPos, micPosCurrent) > SamplesPerFragment) {
            auto remain = _micBuffer.size() - _micBufferPos;

            if (remain < SamplesPerFragment) {
                memcpy(_fragmentBuffer.begin(), _micBuffer.begin() + _micBufferPos, remain * sizeof(float));
                memcpy(_fragmentBuffer.begin() + remain, _micBuffer.begin(), (SamplesPerFragment - remain) * sizeof(float));
            } else {
                memcpy(_fragmentBuffer.begin(), _micBuffer.begin() + _micBufferPos, SamplesPerFragment * sizeof(float));
            }

            onFragmentReadyEvent.invoke(_fragmentBuffer, _captureClip->get_frequency());

            _micBufferPos += SamplesPerFragment;

            if (_micBufferPos > _micBuffer.size())
                _micBufferPos -= _micBuffer.size();
        }
    }

    ArrayW<StringW> MicrophoneManager::GetAvailableDeviceNames(bool ignorePermission) {
        if (!get_hasMicrophonePermission() && !ignorePermission) {
            return ArrayW<StringW>(il2cpp_array_size_t(0));
        }

        auto actualDevices = Microphone::get_devices();
        std::vector<StringW> devices;
        if (actualDevices.size() > 0) {
            // we remove devices we don't want to show, but if the user connects for example a headset or bluetooth, it should still show those
            for (auto device : actualDevices) {
                if (device != "Android camcorder input" && device != "Android voice recognition input") {
                    devices.emplace_back(device);
                }
            }
        }

        return il2cpp_utils::vectorToArray(devices);
    }

    bool MicrophoneManager::TryAutoSelectDevice() {
        if (!get_hasMicrophonePermission()) {
            ERROR("User has not granted microphone permission, not auto selecting device!");
            return false;
        }

        if (!config.microphoneDevice.empty() && TrySelectDevice(config.microphoneDevice))
            return true;

        if (TrySelectDevice(nullptr))
            return true;

        ERROR("No valid recording devices available, will not be able to speak");
        return false;
    }

    bool MicrophoneManager::TrySelectDevice(StringW deviceName) {
        if (!get_hasMicrophonePermission()) {
            ERROR("User has not granted microphone permission, not selecting device!");
            return false;
        }

        if (deviceName && deviceName == "Default") deviceName = nullptr;

        if (get_isCapturing())
            StopCapture();

        if (deviceName && deviceName == "None") {
            _selectedDeviceName = nullptr;
            _haveSelectedDevice = false;
            _minFreq = 0;
            _maxFreq = 0;
            WARNING("Recording device selection removed, will not be able to speak");
            return true;
        }

        if (deviceName && !GetAvailableDeviceNames()->Contains(deviceName)) {
            ERROR("Requested device is not available: {}", deviceName);
            return false;
        }

        _selectedDeviceName = deviceName;
        _haveSelectedDevice = true;

        Microphone::GetDeviceCaps(deviceName, _minFreq, _maxFreq);

        INFO("Selected recording device {} (frequency={})", deviceName ? deviceName : "(Default)", GetRecordingFrequency());
        return true;
    }

    void MicrophoneManager::StartCapture() {
        StopCapture();

        if (!get_haveSelectedDevice())
            throw std::runtime_error("Cannot start capture without a selected device");

        auto recordingFreq = GetRecordingFrequency();

        Microphone::End(get_selectedDeviceName());
        _captureClip = Microphone::Start(get_selectedDeviceName(), true, 1, recordingFreq);

        _micBufferPos = 0;

        if (!_micBuffer || _micBuffer.size() != recordingFreq)
            _micBuffer = ArrayW<float>(il2cpp_array_size_t(recordingFreq));

        _isCapturing = true;
    }

    void MicrophoneManager::StopCapture() {
        if (!get_isCapturing()) return;

        Microphone::End(get_selectedDeviceName());
        _isCapturing = false;

        if (_captureClip && _captureClip->m_CachedPtr.m_value)
            UnityEngine::Object::Destroy(_captureClip);
        _captureClip = nullptr;

        _micBufferPos = 0;
        memset(_fragmentBuffer.begin(), 0, _fragmentBuffer.size() * sizeof(float));

        if (_micBuffer)
            memset(_micBuffer.begin(), 0, _micBuffer.size() * sizeof(float));

        onCaptureEndEvent.invoke();
    }
}
