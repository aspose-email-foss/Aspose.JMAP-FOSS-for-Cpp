#pragma once

#include <string>
#include <vector>
#include <optional>
#include <nlohmann/json.hpp>

#include "EmailAddress.hpp"

namespace aspose_jmap {

/// Represents a group of email addresses as used in From/To headers, e.g. "Team: a@x, b@x;".
struct EmailAddressGroup {
    /// The optional name of the group (may be null).
    std::optional<std::string> name;

    /// The list of email addresses belonging to the group.
    std::vector<EmailAddress> addresses;

    /// Construct an instance from a JSON object following the JMAP wire format.
    /// @throws JmapProtocolError if required fields are missing or have incorrect types.
    static EmailAddressGroup fromJson(const nlohmann::json& data) {
        EmailAddressGroup result;

        // Optional 'name' (String|null)
        if (data.contains("name")) {
            if (!data.at("name").is_null()) {
                if (!data.at("name").is_string()) {
                    throw JmapProtocolError("Invalid type for field 'name': expected string or null");
                }
                result.name = data.at("name").get<std::string>();
            }
        }

        // Required 'addresses' (EmailAddress[])
        if (!data.contains("addresses")) {
            throw JmapProtocolError("Missing required field 'addresses'");
        }
        const auto& addrArray = data.at("addresses");
        if (!addrArray.is_array()) {
            throw JmapProtocolError("Invalid type for field 'addresses': expected array");
        }
        result.addresses.reserve(addrArray.size());
        for (const auto& item : addrArray) {
            result.addresses.emplace_back(EmailAddress::fromJson(item));
        }

        return result;
    }

    /// Serialize this instance to a JSON object following the JMAP wire format.
    nlohmann::json toJson() const {
        nlohmann::json j;
        if (name.has_value()) {
            j["name"] = *name;
        }
        nlohmann::json addrArray = nlohmann::json::array();
        for (const auto& addr : addresses) {
            addrArray.push_back(addr.toJson());
        }
        j["addresses"] = std::move(addrArray);
        return j;
    }
};

} // namespace aspose_jmap
