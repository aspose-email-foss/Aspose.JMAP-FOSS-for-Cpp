#pragma once

#include <string>
#include <vector>
#include <optional>
#include <map>
#include <stdexcept>
#include <nlohmann/json.hpp>

#include "EmailAddress.hpp"
#include "EmailBodyPart.hpp"
#include "CommonTypes.hpp"

namespace aspose_jmap {

/// Represents the value of an email body part (used in Email.bodyValues).
struct EmailBodyValue {
    std::string value;
    bool isEncodingProblem;
    bool isTruncated;

    /// Construct an EmailBodyValue from a JSON object.
    /// @throws JmapProtocolError if required fields are missing or have wrong types.
    static EmailBodyValue fromJson(const nlohmann::json& data) {
        try {
            EmailBodyValue ev;
            ev.value = data.at("value").get<std::string>();
            ev.isEncodingProblem = data.at("isEncodingProblem").get<bool>();
            ev.isTruncated = data.at("isTruncated").get<bool>();
            return ev;
        } catch (const nlohmann::json::exception& e) {
            throw JmapProtocolError(std::string("EmailBodyValue parsing error: ") + e.what());
        }
    }

    /// Serialize this EmailBodyValue to JSON.
    nlohmann::json toJson() const {
        return nlohmann::json{
            {"value", value},
            {"isEncodingProblem", isEncodingProblem},
            {"isTruncated", isTruncated}
        };
    }
};

/// Represents a single email message (RFC 8621 "Email" object).
struct Email {
    // Server‑assigned / immutable fields (optional when constructing a create payload)
    std::optional<std::string> id;
    std::optional<std::string> blobId;
    std::optional<std::string> threadId;
    std::optional<std::uint64_t> size;
    std::optional<std::string> receivedAt;
    std::optional<EmailBodyPart> bodyStructure;
    std::optional<std::map<std::string, EmailBodyValue>> bodyValues;
    std::optional<std::vector<EmailBodyPart>> textBody;
    std::optional<std::vector<EmailBodyPart>> htmlBody;
    std::optional<std::vector<EmailBodyPart>> attachments;
    std::optional<bool> hasAttachment;
    std::optional<std::string> preview;

    // Required mutable per‑mailbox metadata
    std::map<std::string, bool> mailboxIds;
    std::map<std::string, bool> keywords = {};

    // Optional mutable fields (nullable)
    std::optional<std::vector<std::string>> messageId;
    std::optional<std::vector<std::string>> inReplyTo;
    std::optional<std::vector<std::string>> references;
    std::optional<std::vector<EmailAddress>> sender;
    std::optional<std::vector<EmailAddress>> from;
    std::optional<std::vector<EmailAddress>> to;
    std::optional<std::vector<EmailAddress>> cc;
    std::optional<std::vector<EmailAddress>> bcc;
    std::optional<std::vector<EmailAddress>> replyTo;
    std::optional<std::string> subject;
    std::optional<std::string> sentAt;

    /// Construct an Email from a JSON object.
    /// @throws JmapProtocolError if required fields are missing or have wrong types.
    static Email fromJson(const nlohmann::json& data) {
        try {
            Email e;

            // Optional server‑assigned fields
            if (data.contains("id") && !data.at("id").is_null())
                e.id = data.at("id").get<std::string>();
            if (data.contains("blobId") && !data.at("blobId").is_null())
                e.blobId = data.at("blobId").get<std::string>();
            if (data.contains("threadId") && !data.at("threadId").is_null())
                e.threadId = data.at("threadId").get<std::string>();
            if (data.contains("size") && !data.at("size").is_null())
                e.size = data.at("size").get<std::uint64_t>();
            if (data.contains("receivedAt") && !data.at("receivedAt").is_null())
                e.receivedAt = data.at("receivedAt").get<std::string>();
            if (data.contains("bodyStructure") && !data.at("bodyStructure").is_null())
                e.bodyStructure = EmailBodyPart::fromJson(data.at("bodyStructure"));
            if (data.contains("bodyValues") && !data.at("bodyValues").is_null()) {
                std::map<std::string, EmailBodyValue> map;
                for (auto& [k, v] : data.at("bodyValues").items()) {
                    map.emplace(k, EmailBodyValue::fromJson(v));
                }
                e.bodyValues = std::move(map);
            }
            if (data.contains("textBody") && !data.at("textBody").is_null()) {
                std::vector<EmailBodyPart> vec;
                for (const auto& item : data.at("textBody"))
                    vec.emplace_back(EmailBodyPart::fromJson(item));
                e.textBody = std::move(vec);
            }
            if (data.contains("htmlBody") && !data.at("htmlBody").is_null()) {
                std::vector<EmailBodyPart> vec;
                for (const auto& item : data.at("htmlBody"))
                    vec.emplace_back(EmailBodyPart::fromJson(item));
                e.htmlBody = std::move(vec);
            }
            if (data.contains("attachments") && !data.at("attachments").is_null()) {
                std::vector<EmailBodyPart> vec;
                for (const auto& item : data.at("attachments"))
                    vec.emplace_back(EmailBodyPart::fromJson(item));
                e.attachments = std::move(vec);
            }
            if (data.contains("hasAttachment") && !data.at("hasAttachment").is_null())
                e.hasAttachment = data.at("hasAttachment").get<bool>();
            if (data.contains("preview") && !data.at("preview").is_null())
                e.preview = data.at("preview").get<std::string>();

            // Required mailboxIds
            e.mailboxIds = data.at("mailboxIds").get<std::map<std::string, bool>>();

            // Optional keywords (default empty)
            if (data.contains("keywords") && !data.at("keywords").is_null())
                e.keywords = data.at("keywords").get<std::map<std::string, bool>>();

            // Nullable array fields
            auto parseOptionalStringArray = [&](const std::string& name, std::optional<std::vector<std::string>>& out) {
                if (data.contains(name) && !data.at(name).is_null()) {
                    std::vector<std::string> vec;
                    for (const auto& item : data.at(name))
                        vec.emplace_back(item.get<std::string>());
                    out = std::move(vec);
                }
            };
            parseOptionalStringArray("messageId", e.messageId);
            parseOptionalStringArray("inReplyTo", e.inReplyTo);
            parseOptionalStringArray("references", e.references);

            // Nullable EmailAddress array fields
            auto parseOptionalAddressArray = [&](const std::string& name, std::optional<std::vector<EmailAddress>>& out) {
                if (data.contains(name) && !data.at(name).is_null()) {
                    std::vector<EmailAddress> vec;
                    for (const auto& item : data.at(name))
                        vec.emplace_back(EmailAddress::fromJson(item));
                    out = std::move(vec);
                }
            };
            parseOptionalAddressArray("sender", e.sender);
            parseOptionalAddressArray("from", e.from);
            parseOptionalAddressArray("to", e.to);
            parseOptionalAddressArray("cc", e.cc);
            parseOptionalAddressArray("bcc", e.bcc);
            parseOptionalAddressArray("replyTo", e.replyTo);

            // Nullable scalar fields
            if (data.contains("subject") && !data.at("subject").is_null())
                e.subject = data.at("subject").get<std::string>();
            if (data.contains("sentAt") && !data.at("sentAt").is_null())
                e.sentAt = data.at("sentAt").get<std::string>();

            return e;
        } catch (const nlohmann::json::exception& ex) {
            throw JmapProtocolError(std::string("Email parsing error: ") + ex.what());
        }
    }

    /// Serialize this Email to JSON.
    /// Fields that are std::nullopt are omitted.
    nlohmann::json toJson() const {
        nlohmann::json j;

        if (id) j["id"] = *id;
        if (blobId) j["blobId"] = *blobId;
        if (threadId) j["threadId"] = *threadId;
        if (size) j["size"] = *size;
        if (receivedAt) j["receivedAt"] = *receivedAt;
        if (bodyStructure) j["bodyStructure"] = bodyStructure->toJson();
        if (bodyValues) {
            nlohmann::json bv = nlohmann::json::object();
            for (const auto& [k, v] : *bodyValues)
                bv[k] = v.toJson();
            j["bodyValues"] = std::move(bv);
        }
        if (textBody) {
            nlohmann::json arr = nlohmann::json::array();
            for (const auto& part : *textBody)
                arr.push_back(part.toJson());
            j["textBody"] = std::move(arr);
        }
        if (htmlBody) {
            nlohmann::json arr = nlohmann::json::array();
            for (const auto& part : *htmlBody)
                arr.push_back(part.toJson());
            j["htmlBody"] = std::move(arr);
        }
        if (attachments) {
            nlohmann::json arr = nlohmann::json::array();
            for (const auto& part : *attachments)
                arr.push_back(part.toJson());
            j["attachments"] = std::move(arr);
        }
        if (hasAttachment) j["hasAttachment"] = *hasAttachment;
        if (preview) j["preview"] = *preview;

        j["mailboxIds"] = mailboxIds;
        if (!keywords.empty())
            j["keywords"] = keywords;

        if (messageId) j["messageId"] = *messageId;
        if (inReplyTo) j["inReplyTo"] = *inReplyTo;
        if (references) j["references"] = *references;
        if (sender) {
            nlohmann::json arr = nlohmann::json::array();
            for (const auto& a : *sender) arr.push_back(a.toJson());
            j["sender"] = std::move(arr);
        }
        if (from) {
            nlohmann::json arr = nlohmann::json::array();
            for (const auto& a : *from) arr.push_back(a.toJson());
            j["from"] = std::move(arr);
        }
        if (to) {
            nlohmann::json arr = nlohmann::json::array();
            for (const auto& a : *to) arr.push_back(a.toJson());
            j["to"] = std::move(arr);
        }
        if (cc) {
            nlohmann::json arr = nlohmann::json::array();
            for (const auto& a : *cc) arr.push_back(a.toJson());
            j["cc"] = std::move(arr);
        }
        if (bcc) {
            nlohmann::json arr = nlohmann::json::array();
            for (const auto& a : *bcc) arr.push_back(a.toJson());
            j["bcc"] = std::move(arr);
        }
        if (replyTo) {
            nlohmann::json arr = nlohmann::json::array();
            for (const auto& a : *replyTo) arr.push_back(a.toJson());
            j["replyTo"] = std::move(arr);
        }
        if (subject) j["subject"] = *subject;
        if (sentAt) j["sentAt"] = *sentAt;

        return j;
    }
};

} // namespace aspose_jmap
