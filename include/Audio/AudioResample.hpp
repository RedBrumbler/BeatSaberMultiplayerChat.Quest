#pragma once

#include "logging.hpp"
#include <stdexcept>
#include <cmath>
#include <fmt/core.h>

namespace MultiplayerChat::Audio {
    struct AudioResample {
        static int ResampledSampleCount(int sampleCount, int sourceFrequency, int targetFrequency) {
            return int(double(sampleCount) * double(targetFrequency) / double(sourceFrequency));
        }

        static int Resample(float* source, int sourceLength, float* target, int targetLength, int sourceFrequency, int targetFrequency) {
            if (sourceFrequency == targetFrequency)
                throw std::runtime_error("Source and target frequencies cannot be the same");

            // for sampling we should use the reverse ratio
            auto sampleRatio = float(sourceFrequency) / float(targetFrequency);

            auto requiredSize = ResampledSampleCount(sourceLength, sourceFrequency, targetFrequency);
            if (targetLength < requiredSize)
                throw std::runtime_error(fmt::format("Target's length of '{}' does not meet the minimum length of '{}'", targetLength, requiredSize));

            int writtenLength = 0;
            auto remainder = std::fmodf(sampleRatio, 1);
            if (remainder < 0.0001f) { // remainder nearly 0, basically an integer
                int intSampleRatio = int(std::roundf(sampleRatio));
                for (int i = 0; i < targetLength && i * intSampleRatio < sourceLength; i++) {
                    target[i] = source[i * intSampleRatio];
                    writtenLength++;
                }
            } else { // remainder not close to an int, resample normally
                if (targetFrequency > sourceFrequency) { // we need more samples in our buffer! (target > source)
                    for (int i = 0; i < targetLength && std::ceilf(i * sampleRatio) < sourceLength; i++) {
                        auto lower = int(std::floorf(i * sampleRatio));
                        auto upper = int(std::ceilf(i * sampleRatio));
                        auto sample = std::lerp(source[lower], source[upper], remainder);
                        target[i] = sample;
                        writtenLength++;
                    }
                } else { // we need less samples in our buffer! (target < source)
                    for (int i = 0; i < targetLength && std::floorf(i * sampleRatio) < sourceLength; i++) {
                        auto sampleIdx = int(std::floorf(i * sampleRatio));
                        auto sample = source[sampleIdx];
                        target[i] = sample;
                        writtenLength++;
                    }
                }
            }
            return writtenLength;
        }

        static int Resample(ArrayW<float> source, ArrayW<float> target, int sourceFrequency, int targetFrequency) {
            return Resample(source.begin(), source.size(), target.begin(), target.size(), sourceFrequency, targetFrequency);
        }
    };
}
