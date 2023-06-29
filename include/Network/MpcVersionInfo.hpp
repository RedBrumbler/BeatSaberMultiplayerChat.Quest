#pragma once

#include <string>

namespace MultiplayerChat::Network {
    struct MpcVersionInfo {
        static inline const uint32_t ProtocolVersion = 1;
        static inline const std::string SoVersion = VERSION;
#if GIT_MODIFIED
        static inline const std::string GitVersionInfo = GIT_BRANCH "-" GIT_SHORT_COMMIT "-modified";
#else
        static inline const std::string GitVersionInfo = GIT_BRANCH "-" GIT_SHORT_COMMIT;
#endif
    };
}
