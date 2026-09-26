#pragma once

#include <string>
#include <vector>
#include <map>
#include <optional>
#include <nlohmann/json.hpp>
#include "CommonTypes.hpp"

namespace aspose_jmap {

/// SMTP MAIL FROM / RCPT TO envelope for a submission, distinct from the message's own From/To headers.
struct Envelope {
    /// SMTP MAIL FROM address.
    struct Address {
        /// Email address.
        std::string email;
        /// SMTP MAIL/RCPT parameters, e.g. {"RET": "HDRS"}.
        /// The map itself may be null, and each value may be null.
        std::optional<std::map<std::string, std::optional<std::string>>> parameters;

        /// Construct an Address from a JSON object.
        /// @throws JmapProtocolError if required fields are missing or have wrong types.
        static inline Address fromJson(const nlohmann::json& data) {
            try {
                Address addr;
                // required email
                addr.email = data.at("email").get<std::string>();

                // optional parameters
                if (data.contains("parameters") && !data.at("parameters").is_null()) {
                    const auto& paramObj = data.at("parameters");
                    if (!paramObj.is_object()) {
                        throw JmapProtocolError("parameters must be an object");
                    }
                    std::map<std::string, std::optional<std::string>> map;
                    for (auto it = paramObj.begin(); it != paramObj.end(); ++it) {
                        if (it.value().is_null()) {
                            map[it.key()] = std::nullopt;
                        } else if (it.value().is_string()) {
                            map[it.key()] = it.value().get<std::string>();
                        } else {
                            throw JmapProtocolError("parameters values must be string or null");
                        }
                    }
                    addr.parameters = std::move(map);
                } else {
                    addr.parameters = std::nullopt;
                }
                return addr;
            } catch (const nlohmann::json::exception& e) {
                throw JmapProtocolError(std::string("Address parsing error: ") + e.what());
            }
        }

        /// Serialize this Address to JSON.
        inline nlohmann::json toJson() const {
            nlohmann::json j;
            j["email"] = email;
            if (parameters) {
                nlohmann::json paramJson = nlohmann::json::object();
                for (const auto& [k, v] : *parameters) {
                    if (v) {
                        paramJson[k] = *v;
                    } else {
                        paramJson[k] = nullptr;
                    }
                }
                j["parameters"] = paramJson;
            }
            return j;
        }
    };

    Address mailFrom;
    std::vector<Address> rcptTo;

    /// Construct an Envelope from a JSON object.
    /// @throws JmapProtocolError if required fields are missing or have wrong types.
    static inline Envelope fromJson(const nlohmann::json& data) {
        try {
            Envelope env;
            env.mailFrom = Address::fromJson(data.at("mailFrom"));
            const auto& rcptArray = data.at("rcptTo");
            if (!rcptArray.is_array()) {
                throw JmapProtocolError("rcptTo must be an array");
            }
            for (const auto& item : rcptArray) {
                env.rcptTo.emplace_back(Address::fromJson(item));
            }
            return env;
        } catch (const nlohmann::json::exception& e) {
            throw JmapProtocolError(std::string("Envelope parsing error: ") + e.what());
        }
    }

    /// Serialize this Envelope to JSON.
    inline nlohmann::json toJson() const {
        nlohmann::json j;
        j["mailFrom"] = mailFrom.toJson();
        nlohmann::json rcptArray = nlohmann::json::array();
        for (const auto& addr : rcptTo) {
            rcptArray.push_back(addr.toJson());
        }
        j["rcptTo"] = rcptArray;
        return j;
    }
};

} // namespace aspose_jmap
