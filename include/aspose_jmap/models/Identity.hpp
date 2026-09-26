#pragma once

#include <string>
#include <vector>
#include <optional>
#include <nlohmann/json.hpp>

#include "EmailAddress.hpp"
#include "CommonTypes.hpp"

namespace aspose_jmap {

/// A sending identity (name/email/replyTo used as the From when submitting mail);
/// analogous to configuring a MailAddress + display name on Aspose's SmtpClient.
struct Identity {
    /// Server-assigned identifier (omit when constructing a create payload).
    std::optional<std::string> id;

    /// Display name for the identity. Defaults to empty string.
    std::string name;

    /// Primary email address for the identity. Required.
    std::string email;

    /// Optional list of reply‑to addresses.
    std::optional<std::vector<EmailAddress>> replyTo;

    /// Optional list of BCC addresses.
    std::optional<std::vector<EmailAddress>> bcc;

    /// Text signature. Defaults to empty string.
    std::string textSignature;

    /// HTML signature. Defaults to empty string.
    std::string htmlSignature;

    /// Server‑assigned flag indicating whether the identity may be deleted.
    /// Omit when constructing a create payload.
    std::optional<bool> mayDelete;

    /// Construct an Identity from a JSON object, performing validation.
    /// @throws JmapProtocolError if required fields are missing or have incorrect types.
    static Identity fromJson(const nlohmann::json& data) {
        Identity obj;

        // id – optional
        if (data.contains("id")) {
            if (!data.at("id").is_string())
                throw JmapProtocolError("id", "Expected string");
            obj.id = data.at("id").get<std::string>();
        }

        // name – optional, default empty
        obj.name = data.value("name", std::string{});

        // email – required
        try {
            if (!data.at("email").is_string())
                throw JmapProtocolError("email", "Expected string");
            obj.email = data.at("email").get<std::string>();
        } catch (const nlohmann::json::exception&) {
            throw JmapProtocolError("email", "Missing required field");
        }

        // replyTo – optional array of EmailAddress
        if (data.contains("replyTo")) {
            const auto& arr = data.at("replyTo");
            if (!arr.is_array())
                throw JmapProtocolError("replyTo", "Expected array");
            std::vector<EmailAddress> vec;
            vec.reserve(arr.size());
            for (const auto& item : arr) {
                vec.push_back(EmailAddress::fromJson(item));
            }
            obj.replyTo = std::move(vec);
        }

        // bcc – optional array of EmailAddress
        if (data.contains("bcc")) {
            const auto& arr = data.at("bcc");
            if (!arr.is_array())
                throw JmapProtocolError("bcc", "Expected array");
            std::vector<EmailAddress> vec;
            vec.reserve(arr.size());
            for (const auto& item : arr) {
                vec.push_back(EmailAddress::fromJson(item));
            }
            obj.bcc = std::move(vec);
        }

        // textSignature – optional, default empty
        obj.textSignature = data.value("textSignature", std::string{});

        // htmlSignature – optional, default empty
        obj.htmlSignature = data.value("htmlSignature", std::string{});

        // mayDelete – optional boolean
        if (data.contains("mayDelete")) {
            if (!data.at("mayDelete").is_boolean())
                throw JmapProtocolError("mayDelete", "Expected boolean");
            obj.mayDelete = data.at("mayDelete").get<bool>();
        }

        return obj;
    }

    /// Serialize this Identity to a JSON object, omitting absent optional fields.
    nlohmann::json toJson() const {
        nlohmann::json j;

        if (id) j["id"] = *id;
        j["name"] = name;
        j["email"] = email;

        if (replyTo) {
            nlohmann::json arr = nlohmann::json::array();
            for (const auto& ea : *replyTo) {
                arr.push_back(ea.toJson());
            }
            j["replyTo"] = std::move(arr);
        }

        if (bcc) {
            nlohmann::json arr = nlohmann::json::array();
            for (const auto& ea : *bcc) {
                arr.push_back(ea.toJson());
            }
            j["bcc"] = std::move(arr);
        }

        j["textSignature"] = textSignature;
        j["htmlSignature"] = htmlSignature;

        if (mayDelete) j["mayDelete"] = *mayDelete;

        return j;
    }
};

} // namespace aspose_jmap
