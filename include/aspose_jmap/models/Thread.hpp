#pragma once

#include <string>
#include <vector>
#include <optional>
#include <nlohmann/json.hpp>

#include "CommonTypes.hpp"

namespace aspose_jmap {

/// An ordered list of Email ids that make up a conversation.
struct Thread {
    /// Server‑assigned identifier (omit when constructing a create payload).
    std::optional<std::string> id;

    /// Ordered list of Email ids that make up the conversation (omit when constructing a create payload).
    std::optional<std::vector<std::string>> emailIds;

    /// Construct a Thread model from a JSON object.
    /// @throws JmapProtocolError if a required field is missing or has an unexpected type.
    static Thread fromJson(const nlohmann::json& data) {
        Thread result;

        if (data.contains("id")) {
            try {
                result.id = data.at("id").get<std::string>();
            } catch (const nlohmann::json::exception& e) {
                throw JmapProtocolError(std::string("Thread.id: ") + e.what());
            }
        }

        if (data.contains("emailIds")) {
            try {
                result.emailIds = data.at("emailIds").get<std::vector<std::string>>();
            } catch (const nlohmann::json::exception& e) {
                throw JmapProtocolError("invalidProperties", std::string("Thread.emailIds: ") + e.what());
            }
        }

        return result;
    }

    /// Serialize this Thread model to a JSON object.
    /// Fields that are std::nullopt are omitted from the output.
    nlohmann::json toJson() const {
        nlohmann::json j = nlohmann::json::object();

        if (id) {
            j["id"] = *id;
        }
        if (emailIds) {
            j["emailIds"] = *emailIds;
        }

        return j;
    }
};

} // namespace aspose_jmap
