#pragma once

#include <string>
#include <map>
#include <nlohmann/json.hpp>
#include "CommonTypes.hpp"

namespace aspose_jmap {

/// Represents a single JMAP method invocation: [name, arguments, methodCallId].
/// The JSON representation is a three‑element array, not an object.
struct Invocation {
    /// The JMAP method name, e.g. "Mailbox/get".
    std::string name;

    /// The method arguments as an arbitrary JSON object.
    nlohmann::json arguments;

    /// Client‑chosen identifier for this call, echoed back in the response.
    std::string methodCallId;

    /// Deserialize an Invocation from a JSON value.
    /// @throws JmapProtocolError if the JSON is not a 3‑element array or contains
    ///         invalid types for any of the required positions.
    static inline Invocation fromJson(const nlohmann::json& data) {
        if (!data.is_array() || data.size() != 3) {
            throw JmapProtocolError("Invocation must be a JSON array of exactly three elements");
        }

        // name
        const auto& nameJson = data.at(0);
        if (!nameJson.is_string()) {
            throw JmapProtocolError("Invocation[0] (name) must be a string");
        }
        std::string name = nameJson.get<std::string>();

        // arguments
        const auto& argsJson = data.at(1);
        if (!argsJson.is_object()) {
            throw JmapProtocolError("Invocation[1] (arguments) must be a JSON object");
        }
        nlohmann::json arguments = argsJson;

        // methodCallId
        const auto& idJson = data.at(2);
        if (!idJson.is_string()) {
            throw JmapProtocolError("Invocation[2] (methodCallId) must be a string");
        }
        std::string methodCallId = idJson.get<std::string>();

        return Invocation{std::move(name), std::move(arguments), std::move(methodCallId)};
    }

    /// Serialize this Invocation to its JSON representation (a three‑element array).
    inline nlohmann::json toJson() const {
        return nlohmann::json::array({ name, arguments, methodCallId });
    }
};

} // namespace aspose_jmap
