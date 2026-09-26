#pragma once

#include <string>
#include <nlohmann/json.hpp>
#include "CommonTypes.hpp"

namespace aspose_jmap {

/// Per-recipient delivery outcome, keyed by recipient email address on EmailSubmission.deliveryStatus.
struct DeliveryStatus {
    /// SMTP reply string from the server.
    std::string smtpReply;
    /// Delivery status: "queued", "yes", "no", or "unknown".
    std::string delivered;
    /// Display status: "unknown" or "yes".
    std::string displayed;

    /// Construct a DeliveryStatus from a JSON object.
    /// @throws JmapProtocolError if any required field is missing or has an unexpected type.
    static inline DeliveryStatus fromJson(const nlohmann::json& data) {
        try {
            const auto& smtpReplyJson = data.at("smtpReply");
            const auto& deliveredJson = data.at("delivered");
            const auto& displayedJson = data.at("displayed");

            if (!smtpReplyJson.is_string())
                throw JmapProtocolError("smtpReply must be a string");
            if (!deliveredJson.is_string())
                throw JmapProtocolError("delivered must be a string");
            if (!displayedJson.is_string())
                throw JmapProtocolError("displayed must be a string");

            DeliveryStatus result;
            result.smtpReply = smtpReplyJson.get<std::string>();
            result.delivered = deliveredJson.get<std::string>();
            result.displayed = displayedJson.get<std::string>();
            return result;
        } catch (const nlohmann::json::exception& e) {
            // Re‑throw as a protocol error with a clear message.
            throw JmapProtocolError(std::string("Invalid DeliveryStatus JSON: ") + e.what());
        }
    }

    /// Serialize this DeliveryStatus to a JSON object.
    inline nlohmann::json toJson() const {
        nlohmann::json j;
        j["smtpReply"] = smtpReply;
        j["delivered"] = delivered;
        j["displayed"] = displayed;
        return j;
    }
};

} // namespace aspose_jmap
