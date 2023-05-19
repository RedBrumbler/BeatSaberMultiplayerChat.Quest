#pragma once

#include "UnityEngine/AudioSource.hpp"

namespace MultiplayerChat::Audio {
    class ExtendedAudioSource : public UnityEngine::AudioSource {
        public:
            void set_timeSamples(int samples) {
                // TODO: verify icall correctness
                using SetTimeSamples_fun = function_ptr_t<void, UnityEngine::AudioSource*, int>;
                static auto SetTimeSamples = reinterpret_cast<SetTimeSamples_fun>(il2cpp_functions::resolve_icall("UnityEngine.AudioSource::SetSamplePosition"));
                SetTimeSamples(this, samples);
            }

            void set_spatialize(bool spatialize) {
                using SetSpatialize_fun = function_ptr_t<void, UnityEngine::AudioSource*, bool>;
                static auto SetSpatialize = reinterpret_cast<SetSpatialize_fun>(il2cpp_functions::resolve_icall("UnityEngine.AudioSource::set_spatialize"));
                SetSpatialize(this, spatialize);
            }
    };
}
NEED_NO_BOX(::MultiplayerChat::Audio::ExtendedAudioSource);
DEFINE_IL2CPP_ARG_TYPE(::MultiplayerChat::Audio::ExtendedAudioSource*, "UnityEngine", "AudioSource");
