#pragma once

#include <string>
#include <nlohmann/json.hpp>

#include "CommonTypes.hpp"

namespace aspose_jmap {

/// Represents an EmailHeader JMAP data object.
struct EmailHeader {
    /// The header name.
    std::string name;

    /// The header value.
    std::string value;

    /// Constructs an EmailHeader from a JSON object.
    /// @throws JmapProtocolError if a required field is missing or has an unexpected type.
    static inline EmailHeader fromJson(const nlohmann::json& data) {
        EmailHeader result;

        // name (required, String)
        if (!data.contains("name")) {
            throw JmapProtocolError("Missing required field 'name' in EmailHeader");
        }
        if (!data.at("name").is_string()) {
            throw JmapProtocolError("Field 'name' must be a string in EmailHeader");
        }
        result.name = data.at("name").get<std::string>();

        // value (required, String)
        if (!data.contains("value")) {
            throw JmapProtocolError("Missing required field 'value' in EmailHeader");
        }
        if (!data.at("value").is_string()) {
            throw JmapProtocolError("Field 'value' must be a string in EmailHeader");
        }
        result.value = data.at("value").get<std::string>();

        return result;
    }

    /// Serialises this EmailHeader to a JSON object.
    inline nlohmann::json toJson() const {
        nlohmann::json j;
        j["name"] = name;
        j["value"] = value;
        return j;
    }
};

} // namespace aspose_jmap
