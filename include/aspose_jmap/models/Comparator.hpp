#pragma once

#include <string>
#include <optional>
#include <nlohmann/json.hpp>
#include "CommonTypes.hpp"

namespace aspose_jmap {

/// One entry of an Email/query 'sort' argument.
struct Comparator {
    /// The property to sort by (e.g. "receivedAt", "from", "subject", "size").
    std::string property;

    /// Sort direction. True for ascending (default), false for descending.
    bool isAscending = true;

    /// Optional collation identifier.
    std::optional<std::string> collation;

    /// Construct a Comparator from a JSON object.
    /// @throws JmapProtocolError if a required field is missing or has an invalid type.
    static inline Comparator fromJson(const nlohmann::json& data) {
        Comparator result;

        // property – required string
        try {
            result.property = data.at("property").get<std::string>();
        } catch (const nlohmann::json::exception&) {
            throw JmapProtocolError("Missing or invalid required field 'property'");
        }

        // isAscending – optional boolean, defaults to true
        if (data.contains("isAscending")) {
            try {
                result.isAscending = data.at("isAscending").get<bool>();
            } catch (const nlohmann::json::exception&) {
                throw JmapProtocolError("Invalid type for field 'isAscending'");
            }
        }

        // collation – optional string or null
        if (data.contains("collation")) {
            const auto& colVal = data.at("collation");
            if (colVal.is_null()) {
                result.collation = std::nullopt;
            } else {
                try {
                    result.collation = colVal.get<std::string>();
                } catch (const nlohmann::json::exception&) {
                    throw JmapProtocolError("Invalid type for field 'collation'");
                }
            }
        }

        return result;
    }

    /// Serialize this Comparator to a JSON object.
    inline nlohmann::json toJson() const {
        nlohmann::json j;
        j["property"] = property;
        // Only emit isAscending when it differs from the default to keep payload minimal.
        if (!isAscending) {
            j["isAscending"] = isAscending;
        }
        if (collation) {
            j["collation"] = *collation;
        }
        return j;
    }
};

} // namespace aspose_jmap
