#pragma once

#include <string>
#include <vector>
#include <map>
#include <optional>
#include <nlohmann/json.hpp>

#include "Invocation.hpp"
#include "CommonTypes.hpp"

namespace aspose_jmap {

/// Represents the JSON body returned from the JMAP apiUrl.
/// Contains the method responses, optional created IDs map, and the session state.
class JmapResponseEnvelope {
public:
    /// The list of method invocation results.
    std::vector<Invocation> methodResponses;

    /// Optional map of client‑provided creation IDs to server‑assigned IDs.
    std::optional<std::map<std::string, std::string>> createdIds;

    /// The current session state string.
    std::string sessionState;

    /// Construct from a JSON object, performing validation.
    /// @throws JmapProtocolError if required fields are missing or have incorrect types.
    static inline JmapResponseEnvelope fromJson(const nlohmann::json& data) {
        JmapResponseEnvelope result;

        // methodResponses – required array
        if (!data.contains("methodResponses")) {
            throw JmapProtocolError("methodResponses", "Missing required field 'methodResponses'");
        }
        const auto& mrJson = data.at("methodResponses");
        if (!mrJson.is_array()) {
            throw JmapProtocolError("methodResponses", "Field 'methodResponses' must be an array");
        }
        for (const auto& elem : mrJson) {
            result.methodResponses.emplace_back(Invocation::fromJson(elem));
        }

        // createdIds – optional map<String, Id>
        if (data.contains("createdIds") && !data.at("createdIds").is_null()) {
            const auto& cidJson = data.at("createdIds");
            if (!cidJson.is_object()) {
                throw JmapProtocolError("createdIds", "Field 'createdIds' must be an object when present");
            }
            std::map<std::string, std::string> map;
            for (auto it = cidJson.begin(); it != cidJson.end(); ++it) {
                if (!it.value().is_string()) {
                    throw JmapProtocolError("createdIds", "All values in 'createdIds' must be strings");
                }
                map.emplace(it.key(), it.value().get<std::string>());
            }
            result.createdIds = std::move(map);
        }

        // sessionState – required string
        if (!data.contains("sessionState")) {
            throw JmapProtocolError("sessionState", "Missing required field 'sessionState'");
        }
        const auto& ssJson = data.at("sessionState");
        if (!ssJson.is_string()) {
            throw JmapProtocolError("sessionState", "Field 'sessionState' must be a string");
        }
        result.sessionState = ssJson.get<std::string>();

        return result;
    }

    /// Serialize this envelope to a JSON object.
    /// Optional fields that are not set are omitted.
    inline nlohmann::json toJson() const {
        nlohmann::json j;
        // methodResponses
        nlohmann::json mrArray = nlohmann::json::array();
        for (const auto& inv : methodResponses) {
            mrArray.push_back(inv.toJson());
        }
        j["methodResponses"] = std::move(mrArray);

        // createdIds – only if present
        if (createdIds) {
            nlohmann::json cidObj = nlohmann::json::object();
            for (const auto& [k, v] : *createdIds) {
                cidObj[k] = v;
            }
            j["createdIds"] = std::move(cidObj);
        }

        // sessionState
        j["sessionState"] = sessionState;
        return j;
    }
};

} // namespace aspose_jmap
