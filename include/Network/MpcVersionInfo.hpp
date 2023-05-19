#pragma once

#include <string>

namespace MultiplayerChat::Network {
    struct MpcVersionInfo {
        static inline const uint32_t ProtocolVersion = 1;
        static inline const std::string SoVersion = VERSION;
    };
}
