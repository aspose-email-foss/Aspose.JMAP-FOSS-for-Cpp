#pragma once

#include <string>
#include <optional>
#include <nlohmann/json.hpp>
#include "CommonTypes.hpp"

namespace aspose_jmap {

/// One address in a header such as From/To/Cc (RFC 8621 section 4.1.2.3).
struct EmailAddress {
    /// Optional display name.
    std::optional<std::string> name;
    /// Required email address.
    std::string email;

    /// Construct an EmailAddress from a JSON object.
    /// @throws JmapProtocolError if required fields are missing or have incorrect types.
    static inline EmailAddress fromJson(const nlohmann::json& data) {
        EmailAddress result;

        // email – required string
        if (!data.contains("email")) {
            throw JmapProtocolError("email", "Missing required field 'email'");
        }
        const auto& emailJson = data.at("email");
        if (!emailJson.is_string()) {
            throw JmapProtocolError("email", "Field 'email' must be a string");
        }
        result.email = emailJson.get<std::string>();

        // name – optional string or null
        if (data.contains("name")) {
            const auto& nameJson = data.at("name");
            if (!nameJson.is_null()) {
                if (!nameJson.is_string()) {
                    throw JmapProtocolError("name", "Field 'name' must be a string or null");
                }
                result.name = nameJson.get<std::string>();
            } else {
                result.name = std::nullopt;
            }
        }

        return result;
    }

    /// Serialize this EmailAddress to a JSON object.
    inline nlohmann::json toJson() const {
        nlohmann::json j;
        if (name.has_value()) {
            j["name"] = *name;
        }
        j["email"] = email;
        return j;
    }
};

} // namespace aspose_jmap
