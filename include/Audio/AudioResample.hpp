#pragma once

#include <stdexcept>
#include <cmath>
#include <fmt/core.h>

namespace MultiplayerChat::Audio {
    struct AudioResample {
        static int Resample(float* sourceBegin, float* sourceEnd, float* targetBegin, float* targetEnd, int sourceFrequency, int outputFrequency, int outputChannels = 1) {
            if (sourceFrequency == outputFrequency)
                throw std::runtime_error("Source and target frequencies cannot be the same");

            auto ratio = float(sourceFrequency) / float(outputFrequency);

            auto sourceLength = sourceEnd - sourceBegin;
            auto targetLength = targetEnd - targetBegin;

            auto length = 0;

            auto remainder = std::fmodf(ratio, 1);
            if (remainder <= 0.0001f) { // it's close to just being an integer, so just cast it to an int

                auto intRatio = (int)std::roundf(ratio);
                auto sizeRequired = sourceLength * intRatio * outputChannels;

                if (sizeRequired > targetLength)
                    throw std::runtime_error(fmt::format("Target's length of '{}' does not meet the minimum length of '{}'", targetLength, sizeRequired));

                int perChannelTargetLength = targetLength / outputChannels;
                for (int i = 0; i < perChannelTargetLength && (i * intRatio) < sourceLength; i++) {
                    for (int j = 0; j < outputChannels; j++) {
                        auto targetIndex = i * outputChannels + j;
                        auto sourceSample = sourceBegin[i * intRatio];
                        targetBegin[targetIndex] = sourceSample;
                        length++;
                    }
                }
            } else { // we're not close to being an integer
                if (ratio > 1) { // downsampling, output < source
                    auto sizeRequired = int(std::ceilf(sourceLength * ratio));

                    if (sizeRequired > targetLength)
                        throw std::runtime_error(fmt::format("Target's length of '{}' does not meet the minimum length of '{}'", targetLength, sizeRequired));

                    int perChannelTargetLength = targetLength / outputChannels;
                    for (int i = 0; i < perChannelTargetLength && std::ceilf(i * ratio) < sourceLength; i++) {
                        auto sourceIdx = i * ratio;
                        for (int j = 0; j < outputChannels; j++) {
                            auto sourceSample = std::lerp(
                                sourceBegin[(int)std::floorf(sourceIdx)],
                                sourceBegin[(int)std::ceilf(sourceIdx)],
                                remainder);

                            auto targetIndex = i * outputChannels + j;
                            targetBegin[targetIndex] = sourceSample;
                            length++;
                        }
                    }
                } else { // upsampling, output > source
                    auto sizeRequired = int(std::floorf(sourceLength * ratio));

                    if (sizeRequired > targetLength)
                        throw std::runtime_error(fmt::format("Target's length of '{}' does not meet the minimum length of '{}'", targetLength, sizeRequired));

                    int perChannelTargetLength = targetLength / outputChannels;
                    for (int i = 0; i < perChannelTargetLength && std::floorf(i * ratio) < sourceLength; i++) {
                        auto sourceIdx = i * ratio;
                        for (int j = 0; j < outputChannels; j++) {
                            auto sourceSample = sourceBegin[(int)std::floorf(sourceIdx)];

                            auto targetIndex = i * outputChannels + j;
                            targetBegin[targetIndex] = sourceSample;
                            length++;
                        }
                    }
                }
            }

            return length;
        }

        static int Resample(ArrayW<float> source, ArrayW<float> target, int sourceFrequency, int outputFrequency, int outputChannels = 1) {
            return Resample(source.begin(), source.end(), target.begin(), target.end(), sourceFrequency, outputFrequency, outputChannels);
        }
    };
}
