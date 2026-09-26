#pragma once

#include <string>
#include <vector>
#include <map>
#include <optional>
#include <stdexcept>

#include <nlohmann/json.hpp>

#include "DeliveryStatus.hpp"
#include "Envelope.hpp"
#include "CommonTypes.hpp"

namespace aspose_jmap {

/// One attempt to submit an Email for delivery. Analogous to what Aspose.Email's SmtpClient::Send
/// does synchronously over SMTP; here creating an EmailSubmission is what actually dispatches the
/// message via the server's outbound MTA.
struct EmailSubmission {
    // Server-assigned fields (optional when constructing a create payload)
    std::optional<std::string> id;                                   ///< Id (server-assigned)
    std::optional<std::string> threadId;                             ///< Id (server-assigned)
    std::optional<std::string> sendAt;                               ///< UTCDate (server-assigned)
    std::optional<std::string> undoStatus;                           ///< String (server-assigned)
    std::optional<std::map<std::string, DeliveryStatus>> deliveryStatus; ///< Map<String, DeliveryStatus>|null
    std::optional<std::vector<std::string>> dsnBlobIds;             ///< Id[]
    std::optional<std::vector<std::string>> mdnBlobIds;             ///< Id[]

    // Required client‑provided fields
    std::string identityId;                                           ///< Id (required)
    std::string emailId;                                              ///< Id (required)

    // Optional nullable field
    std::optional<Envelope> envelope;                                 ///< Envelope|null

    /// Construct an EmailSubmission from a JSON object.
    /// @throws JmapProtocolError if a required field is missing or has an unexpected type.
    static EmailSubmission fromJson(const nlohmann::json& data) {
        EmailSubmission result;

        // Required fields
        try {
            result.identityId = data.at("identityId").get<std::string>();
        } catch (const std::exception& e) {
            throw JmapProtocolError("identityId", "Missing or invalid required field 'identityId'");
        }
        try {
            result.emailId = data.at("emailId").get<std::string>();
        } catch (const std::exception& e) {
            throw JmapProtocolError("emailId", "Missing or invalid required field 'emailId'");
        }

        // Optional server-assigned fields
        if (data.contains("id") && !data.at("id").is_null())
            result.id = data.at("id").get<std::string>();

        if (data.contains("threadId") && !data.at("threadId").is_null())
            result.threadId = data.at("threadId").get<std::string>();

        if (data.contains("sendAt") && !data.at("sendAt").is_null())
            result.sendAt = data.at("sendAt").get<std::string>();

        if (data.contains("undoStatus") && !data.at("undoStatus").is_null())
            result.undoStatus = data.at("undoStatus").get<std::string>();

        // envelope (nullable)
        if (data.contains("envelope")) {
            if (!data.at("envelope").is_null())
                result.envelope = Envelope::fromJson(data.at("envelope"));
            else
                result.envelope = std::nullopt;
        }

        // deliveryStatus (nullable map)
        if (data.contains("deliveryStatus")) {
            if (!data.at("deliveryStatus").is_null()) {
                std::map<std::string, DeliveryStatus> map;
                for (auto& [k, v] : data.at("deliveryStatus").items()) {
                    map.emplace(k, DeliveryStatus::fromJson(v));
                }
                result.deliveryStatus = std::move(map);
            } else {
                result.deliveryStatus = std::nullopt;
            }
        }

        // dsnBlobIds
        if (data.contains("dsnBlobIds") && !data.at("dsnBlobIds").is_null()) {
            std::vector<std::string> vec;
            for (const auto& el : data.at("dsnBlobIds"))
                vec.emplace_back(el.get<std::string>());
            result.dsnBlobIds = std::move(vec);
        }

        // mdnBlobIds
        if (data.contains("mdnBlobIds") && !data.at("mdnBlobIds").is_null()) {
            std::vector<std::string> vec;
            for (const auto& el : data.at("mdnBlobIds"))
                vec.emplace_back(el.get<std::string>());
            result.mdnBlobIds = std::move(vec);
        }

        return result;
    }

    /// Serialize this EmailSubmission to a JSON object.
    /// Only fields that have a value are emitted.
    nlohmann::json toJson() const {
        nlohmann::json j = nlohmann::json::object();

        if (id)               j["id"] = *id;
        j["identityId"] = identityId;
        j["emailId"] = emailId;
        if (threadId)         j["threadId"] = *threadId;
        if (envelope)         j["envelope"] = envelope->toJson();
        if (sendAt)           j["sendAt"] = *sendAt;
        if (undoStatus)       j["undoStatus"] = *undoStatus;
        if (deliveryStatus) {
            nlohmann::json map = nlohmann::json::object();
            for (const auto& [k, v] : *deliveryStatus)
                map[k] = v.toJson();
            j["deliveryStatus"] = std::move(map);
        }
        if (dsnBlobIds)       j["dsnBlobIds"] = *dsnBlobIds;
        if (mdnBlobIds)       j["mdnBlobIds"] = *mdnBlobIds;

        return j;
    }
};

} // namespace aspose_jmap
