#include "Audio/SoundNotifier.hpp"
#include "Utilities/FileUtils.hpp"
#include "config.hpp"
#include "logging.hpp"

#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/AudioType.hpp"
#include "UnityEngine/Networking/DownloadHandlerAudioClip.hpp"
#include "UnityEngine/Networking/UnityWebRequestMultimedia.hpp"
#include "UnityEngine/Networking/UnityWebRequest.hpp"

#include <filesystem>

using namespace MultiplayerChat::Utilities;

DEFINE_TYPE(MultiplayerChat::Audio, SoundNotifier);

namespace MultiplayerChat::Audio {
    void SoundNotifier::ctor() {
        INVOKE_CTOR();

        _directoryPath = "/sdcard/ModData/com.beatgames.beatsaber/Mods/MultiplayerChat/Sounds";
        _loadedClips = AudioClipDict::New_ctor();
        _audioSource = nullptr;
        _previewMode = false;
    }

    void SoundNotifier::Initialize() { mkpath(get_directoryPath()); }

    void SoundNotifier::Awake() {
        _audioSource = GetComponent<ExtendedAudioSource*>();
        if (!_audioSource) _audioSource = get_gameObject()->AddComponent<ExtendedAudioSource*>();
        _previewMode = false;

        LoadConfiguredClip();
    }

    void SoundNotifier::Dispose() {
        if (_audioSource && _audioSource->m_CachedPtr.m_value) {
            _audioSource->Stop();
        }

        auto enumerator = _loadedClips->GetEnumerator();
        while (enumerator.MoveNext()) {
            auto clip = enumerator.get_Current().get_Value();
            if (clip && clip->m_CachedPtr.m_value)
                UnityEngine::Object::Destroy(clip);
        }
        enumerator.Dispose();

        _loadedClips->Clear();
    }

    void SoundNotifier::Play() {
        if (config.soundNotification.empty()) return;
        Play(config.soundNotification);
    }

    void SoundNotifier::Play(std::string clipName) {
        if (clipName == "None") return;
        _previewMode = false;

        if (!_audioSource || !_audioSource->m_CachedPtr.m_value) return;

        if (!clipName.ends_with(".ogg")) clipName += ".ogg";

        UnityEngine::AudioClip* audioClip = nullptr;
        if (!_loadedClips->TryGetValue(clipName, byref(audioClip))) {
            WARNING("Can't play audio clip because it's not loaded: {}", clipName);
            return;
        }

        _audioSource->PlayOneShot(audioClip, config.soundNotificationVolume);
    }

    void SoundNotifier::LoadClipIfNeeded(std::string clipName) {
        if (!get_isActiveAndEnabled()) return;
        if (clipName == "None") return;
        if (!clipName.ends_with(".ogg")) clipName += ".ogg";
        if (_loadedClips->ContainsKey(clipName)) return;

        StartCoroutine(custom_types::Helpers::CoroutineHelper::New(LoadClipRoutine(clipName)));
    }

    void SoundNotifier::LoadAndPlayPreview(std::string clipName) {
        if (!get_isActiveAndEnabled()) return;
        if (clipName == "None") return;
        if (!clipName.ends_with(".ogg")) clipName += ".ogg";

        if (_loadedClips->ContainsKey(clipName)) {
            Play(clipName);
            return;
        }

        _previewMode = true;
        StartCoroutine(custom_types::Helpers::CoroutineHelper::New(LoadClipRoutine(clipName)));
    }

    void SoundNotifier::LoadConfiguredClip() {
        if (!get_isActiveAndEnabled()) return;
        if (config.soundNotification.empty() || config.soundNotification == "None" || config.soundNotificationVolume <= 0) {
            DEBUG("Sound notification is disabled in config");
            return;
        }

        StartCoroutine(custom_types::Helpers::CoroutineHelper::New(LoadClipRoutine(config.soundNotification)));
    }

    std::vector<std::string> SoundNotifier::GetAvailableClipNames() {
        std::vector<std::string> ret;
        FileUtils::GetFilesInFolderPath("ogg", get_directoryPath(), ret);
        return ret;
    }

    custom_types::Helpers::Coroutine SoundNotifier::LoadClipRoutine(std::string clipName) {
        if (clipName == "None") co_return;
        if (!clipName.ends_with(".ogg")) clipName += ".ogg";

        if (_loadedClips->ContainsKey(clipName)) {
            WARNING("Skipping duplciate clip: {}", clipName);
            co_return;
        }

        auto clipPath = fmt::format("{}/{}", get_directoryPath(), clipName);
        if (!fileexists(clipPath)) {
            ERROR("Sound name must refer to valid .ogg file on disk (tried {})", clipPath);
            co_return;
        }

        using namespace UnityEngine::Networking;
        auto req = UnityWebRequestMultimedia::GetAudioClip(fmt::format("file://{}", clipPath), UnityEngine::AudioType::OGGVORBIS);
        co_yield reinterpret_cast<System::Collections::IEnumerator*>(req);

        if (!Il2CppString::IsNullOrEmpty(req->get_error())) {
            ERROR("Error trying to load {}: {}", clipPath, req->get_error());
            co_return;
        }

        auto clip = DownloadHandlerAudioClip::GetContent(req);
        if (!clip) {
            ERROR("Error trying to load {}: failed to get clip", clipPath);
            co_return;
        }

        _loadedClips->set_Item(clipName, clip);
        DEBUG("Loaded clip {}", clipName);

        if (_previewMode)
            Play(clipName);

        co_return;
    }

}
