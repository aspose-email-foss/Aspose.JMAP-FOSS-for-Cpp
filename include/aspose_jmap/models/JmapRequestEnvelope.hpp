#pragma once

#include "Invocation.hpp"
#include "CommonTypes.hpp"
#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <map>
#include <optional>

namespace aspose_jmap {

/// Represents the JMAP request envelope sent to the Session API URL.
/// Contains the required capability URNs, the list of method invocations,
/// and an optional map of client‑generated IDs to server‑assigned IDs.
struct JmapRequestEnvelope {
    /// Capability URNs this request depends on (must include urn:ietf:params:jmap:core).
    std::vector<std::string> using_;

    /// The method calls to invoke.
    std::vector<Invocation> methodCalls;

    /// Optional map of client‑generated IDs to server‑assigned IDs.
    std::optional<std::map<std::string, std::string>> createdIds;

    /// Construct a model instance from a JSON object.
    /// @throws JmapProtocolError if required fields are missing or have an unexpected type.
    static inline JmapRequestEnvelope fromJson(const nlohmann::json& data) {
        try {
            JmapRequestEnvelope env;

            // required: using
            const auto& usingJson = data.at("using");
            if (!usingJson.is_array())
                throw std::runtime_error("'using' must be an array");
            env.using_.reserve(usingJson.size());
            for (const auto& item : usingJson) {
                env.using_.push_back(item.get<std::string>());
            }

            // required: methodCalls
            const auto& callsJson = data.at("methodCalls");
            if (!callsJson.is_array())
                throw std::runtime_error("'methodCalls' must be an array");
            env.methodCalls.reserve(callsJson.size());
            for (const auto& call : callsJson) {
                env.methodCalls.push_back(Invocation::fromJson(call));
            }

            // optional: createdIds
            if (data.contains("createdIds") && !data.at("createdIds").is_null()) {
                const auto& idsJson = data.at("createdIds");
                if (!idsJson.is_object())
                    throw std::runtime_error("'createdIds' must be an object or null");
                std::map<std::string, std::string> idsMap;
                for (auto it = idsJson.begin(); it != idsJson.end(); ++it) {
                    idsMap[it.key()] = it.value().get<std::string>();
                }
                env.createdIds = std::move(idsMap);
            }

            return env;
        } catch (const std::exception& e) {
            throw JmapProtocolError(std::string("JmapRequestEnvelope parsing error: ") + e.what());
        }
    }

    /// Serialize this envelope to JSON.
    inline nlohmann::json toJson() const {
        nlohmann::json j;
        j["using"] = using_;
        nlohmann::json calls = nlohmann::json::array();
        for (const auto& call : methodCalls) {
            calls.push_back(call.toJson());
        }
        j["methodCalls"] = std::move(calls);
        if (createdIds) {
            nlohmann::json idsJson = nlohmann::json::object();
            for (const auto& [k, v] : *createdIds) {
                idsJson[k] = v;
            }
            j["createdIds"] = std::move(idsJson);
        }
        return j;
    }
};

} // namespace aspose_jmap
