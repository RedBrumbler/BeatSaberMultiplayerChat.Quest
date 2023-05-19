#pragma once

#include "custom-types/shared/macros.hpp"
#include "custom-types/shared/coroutine.hpp"
#include "UnityEngine/MonoBehaviour.hpp"

#include "Zenject/IInitializable.hpp"
#include "System/IDisposable.hpp"

#include "UnityEngine/AudioClip.hpp"
#include "System/Collections/Generic/Dictionary_2.hpp"
#include "ExtendedAudioSource.hpp"

DECLARE_CLASS_CODEGEN_INTERFACES(MultiplayerChat::Audio, SoundNotifier, UnityEngine::MonoBehaviour, std::vector<Il2CppClass*>({classof(Zenject::IInitializable*), classof(System::IDisposable*)}),
    DECLARE_INSTANCE_FIELD_PRIVATE(ExtendedAudioSource*, _audioSource);

    using AudioClipDict = System::Collections::Generic::Dictionary_2<StringW, UnityEngine::AudioClip*>;
    DECLARE_INSTANCE_FIELD_PRIVATE(AudioClipDict*, _loadedClips);
    DECLARE_INSTANCE_FIELD_PRIVATE(bool, _previewMode);

    DECLARE_OVERRIDE_METHOD(void, Dispose, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::System::IDisposable::Dispose>::get());
    DECLARE_OVERRIDE_METHOD(void, Initialize, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::Zenject::IInitializable::Initialize>::get());

    DECLARE_INSTANCE_METHOD(void, Awake);
    DECLARE_CTOR(ctor);
    public:
        std::string_view get_directoryPath() const { return _directoryPath; }
        std::vector<std::string> GetAvailableClipNames();
        void Play();
        void Play(std::string clipName);
        void LoadClipIfNeeded(std::string clipName);
        void LoadAndPlayPreview(std::string clipName);

    private:
        void LoadConfiguredClip();
        custom_types::Helpers::Coroutine LoadClipRoutine(std::string clipName);

        std::string _directoryPath;
)
