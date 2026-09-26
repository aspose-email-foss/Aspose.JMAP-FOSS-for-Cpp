#pragma once

#include <string>
#include <vector>
#include <optional>
#include <cstdint>
#include <nlohmann/json.hpp>
#include "CommonTypes.hpp"

namespace aspose_jmap {

/// Response shape of the JMAP `Email/query` method (RFC 8620 section 5.5).
struct EmailQueryResponse {
    /// The id of the account used for the call.
    std::string accountId;

    /// A string encoding the current state of the query results.
    std::string queryState;

    /// Whether the server supports Email/queryChanges for this query.
    bool canCalculateChanges = false;

    /// The zero-based index of the first result in 'ids' within the full result set.
    std::uint64_t position = 0;

    /// The list of Email ids matching the query (or a page thereof).
    std::vector<std::string> ids;

    /// Optional total number of results found, if requested and calculable.
    std::optional<std::uint64_t> total;

    /// Optional limit applied by the server to the number of results returned.
    std::optional<std::uint64_t> limit;

    /// Construct an EmailQueryResponse from a JSON object.
    /// @throws JmapProtocolError if a required field is missing or has an unexpected type.
    static inline EmailQueryResponse fromJson(const nlohmann::json& data) {
        EmailQueryResponse result;

        // accountId – required string
        if (!data.contains("accountId")) {
            throw JmapProtocolError("Missing required field: accountId");
        }
        if (!data.at("accountId").is_string()) {
            throw JmapProtocolError("Field 'accountId' must be a string");
        }
        result.accountId = data.at("accountId").get<std::string>();

        // queryState – required string
        if (!data.contains("queryState")) {
            throw JmapProtocolError("Missing required field: queryState");
        }
        if (!data.at("queryState").is_string()) {
            throw JmapProtocolError("Field 'queryState' must be a string");
        }
        result.queryState = data.at("queryState").get<std::string>();

        // canCalculateChanges – required bool
        if (!data.contains("canCalculateChanges")) {
            throw JmapProtocolError("Missing required field: canCalculateChanges");
        }
        if (!data.at("canCalculateChanges").is_boolean()) {
            throw JmapProtocolError("Field 'canCalculateChanges' must be a boolean");
        }
        result.canCalculateChanges = data.at("canCalculateChanges").get<bool>();

        // position – required non-negative integer
        if (!data.contains("position")) {
            throw JmapProtocolError("Missing required field: position");
        }
        if (!data.at("position").is_number_unsigned()) {
            throw JmapProtocolError("Field 'position' must be a non-negative integer");
        }
        result.position = data.at("position").get<std::uint64_t>();

        // ids – required array of strings
        if (!data.contains("ids")) {
            throw JmapProtocolError("Missing required field: ids");
        }
        if (!data.at("ids").is_array()) {
            throw JmapProtocolError("Field 'ids' must be an array");
        }
        result.ids.reserve(data.at("ids").size());
        for (const auto& id : data.at("ids")) {
            if (!id.is_string()) {
                throw JmapProtocolError("Field 'ids' must contain only strings");
            }
            result.ids.emplace_back(id.get<std::string>());
        }

        // total – optional integer (may be absent or null)
        if (data.contains("total") && !data.at("total").is_null()) {
            if (!data.at("total").is_number_unsigned()) {
                throw JmapProtocolError("Field 'total' must be a non-negative integer");
            }
            result.total = data.at("total").get<std::uint64_t>();
        } else {
            result.total = std::nullopt;
        }

        // limit – optional integer (may be absent or null)
        if (data.contains("limit") && !data.at("limit").is_null()) {
            if (!data.at("limit").is_number_unsigned()) {
                throw JmapProtocolError("Field 'limit' must be a non-negative integer");
            }
            result.limit = data.at("limit").get<std::uint64_t>();
        } else {
            result.limit = std::nullopt;
        }

        return result;
    }

    /// Serialize this EmailQueryResponse to a JSON object.
    inline nlohmann::json toJson() const {
        nlohmann::json j = nlohmann::json::object();
        j["accountId"] = accountId;
        j["queryState"] = queryState;
        j["canCalculateChanges"] = canCalculateChanges;
        j["position"] = position;
        j["ids"] = ids;
        if (total.has_value()) {
            j["total"] = total.value();
        }
        if (limit.has_value()) {
            j["limit"] = limit.value();
        }
        return j;
    }
};

} // namespace aspose_jmap
