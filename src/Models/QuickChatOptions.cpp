#include "Models/QuickChatOptions.hpp"
#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"
#include "assets.hpp"
#include "logging.hpp"

#include "beatsaber-hook/shared/rapidjson/include/rapidjson/error/en.h"
DEFINE_TYPE(MultiplayerChat::Models, QuickChatOptions);

namespace MultiplayerChat::Models {
    void QuickChatOptions::ctor() {
        INVOKE_CTOR();
    }

    void QuickChatOptions::Initialize() {
        if (!fileexists(QuickChatOptionsPath)) WriteOutDefault();

        auto json = readfile(QuickChatOptionsPath);
        DEBUG("file read: {}", json);

        rapidjson::Document doc;
        doc.SetObject();
        doc.Parse(json);

        if (doc.HasParseError()) {
            ERROR("Could not parse quickchat.json: {} @ {}", rapidjson::GetParseError_En(doc.GetParseError()), doc.GetErrorOffset());
            return;
        }

        for (auto itr = doc.MemberBegin(); itr < doc.MemberEnd(); itr++) {
            auto& vec = _options[{itr->name.GetString(), itr->name.GetStringLength()}];

            for (auto& member : itr->value.GetArray()) {
                vec.emplace_back(member.GetString(), member.GetStringLength());
            }
        }
    }

    void QuickChatOptions::WriteOutDefault() {
        if (fileexists(QuickChatOptionsPath)) return;
        std::string path(QuickChatOptionsPath);
        mkpath(path.substr(0, path.find_last_of('/')));

        writefile(QuickChatOptionsPath, Assets::quickchat_json);
    }
}
