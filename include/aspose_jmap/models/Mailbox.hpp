#pragma once

#include <string>
#include <optional>
#include <cstdint>
#include <map>
#include <nlohmann/json.hpp>
#include "CommonTypes.hpp"

namespace aspose_jmap {

/// Represents the rights a user has on a mailbox.
struct MailboxRights {
    bool mayReadItems = false;
    bool mayAddItems = false;
    bool mayRemoveItems = false;
    bool maySetSeen = false;
    bool maySetKeywords = false;
    bool mayCreateChild = false;
    bool mayRename = false;
    bool mayDelete = false;
    bool maySubmit = false;

    /// Construct a MailboxRights from a JSON object.
    /// @throws JmapProtocolError if any required field is missing or has the wrong type.
    static inline MailboxRights fromJson(const nlohmann::json& data) {
        MailboxRights r;
        try {
            r.mayReadItems   = data.at("mayReadItems").get<bool>();
            r.mayAddItems    = data.at("mayAddItems").get<bool>();
            r.mayRemoveItems = data.at("mayRemoveItems").get<bool>();
            r.maySetSeen     = data.at("maySetSeen").get<bool>();
            r.maySetKeywords = data.at("maySetKeywords").get<bool>();
            r.mayCreateChild = data.at("mayCreateChild").get<bool>();
            r.mayRename      = data.at("mayRename").get<bool>();
            r.mayDelete      = data.at("mayDelete").get<bool>();
            r.maySubmit      = data.at("maySubmit").get<bool>();
        } catch (const nlohmann::json::exception& e) {
            throw JmapProtocolError(std::string("MailboxRights parsing error: ") + e.what());
        }
        return r;
    }

    /// Convert this MailboxRights to a JSON object.
    inline nlohmann::json toJson() const {
        return nlohmann::json{
            {"mayReadItems",   mayReadItems},
            {"mayAddItems",    mayAddItems},
            {"mayRemoveItems", mayRemoveItems},
            {"maySetSeen",     maySetSeen},
            {"maySetKeywords", maySetKeywords},
            {"mayCreateChild", mayCreateChild},
            {"mayRename",      mayRename},
            {"mayDelete",      mayDelete},
            {"maySubmit",      maySubmit}
        };
    }
};

/// A named set of Emails (JMAP's analogue of a mail folder / IMAP mailbox).
struct Mailbox {
    // Server‑assigned identifier (absent when creating a new mailbox)
    std::optional<std::string> id;

    // Required name of the mailbox
    std::string name;

    // Optional parent mailbox identifier (nullable)
    std::optional<std::string> parentId;

    // Optional role (nullable), e.g. "inbox", "sent", etc.
    std::optional<std::string> role;

    // Sort order, defaults to 0
    std::uint64_t sortOrder = 0;

    // Server‑assigned statistics (absent when creating)
    std::optional<std::uint64_t> totalEmails;
    std::optional<std::uint64_t> unreadEmails;
    std::optional<std::uint64_t> totalThreads;
    std::optional<std::uint64_t> unreadThreads;

    // Server‑assigned rights (absent when creating)
    std::optional<MailboxRights> myRights;

    // Subscription flag, defaults to false
    bool isSubscribed = false;

    /// Construct a Mailbox from a JSON object.
    /// @throws JmapProtocolError if any required field is missing or has the wrong type.
    static inline Mailbox fromJson(const nlohmann::json& data) {
        Mailbox m;

        // Required field: name
        if (!data.contains("name")) {
            throw JmapProtocolError("Missing required field: name");
        }
        try {
            m.name = data.at("name").get<std::string>();
        } catch (const nlohmann::json::exception& e) {
            throw JmapProtocolError(std::string("Invalid type for field 'name': ") + e.what());
        }

        // Optional fields
        if (data.contains("id")) {
            m.id = data.at("id").get<std::string>();
        }
        if (data.contains("parentId")) {
            if (data.at("parentId").is_null()) {
                m.parentId = std::nullopt;
            } else {
                m.parentId = data.at("parentId").get<std::string>();
            }
        }
        if (data.contains("role")) {
            if (data.at("role").is_null()) {
                m.role = std::nullopt;
            } else {
                m.role = data.at("role").get<std::string>();
            }
        }
        if (data.contains("sortOrder")) {
            m.sortOrder = data.at("sortOrder").get<std::uint64_t>();
        }
        if (data.contains("totalEmails")) {
            m.totalEmails = data.at("totalEmails").get<std::uint64_t>();
        }
        if (data.contains("unreadEmails")) {
            m.unreadEmails = data.at("unreadEmails").get<std::uint64_t>();
        }
        if (data.contains("totalThreads")) {
            m.totalThreads = data.at("totalThreads").get<std::uint64_t>();
        }
        if (data.contains("unreadThreads")) {
            m.unreadThreads = data.at("unreadThreads").get<std::uint64_t>();
        }
        if (data.contains("myRights") && !data.at("myRights").is_null()) {
            m.myRights = MailboxRights::fromJson(data.at("myRights"));
        }
        if (data.contains("isSubscribed")) {
            m.isSubscribed = data.at("isSubscribed").get<bool>();
        }

        return m;
    }

    /// Convert this Mailbox to a JSON object.
    inline nlohmann::json toJson() const {
        nlohmann::json j;

        if (id)               j["id"] = *id;
        j["name"] = name;
        if (parentId)         j["parentId"] = *parentId;
        if (role)             j["role"] = *role;
        j["sortOrder"] = sortOrder;
        if (totalEmails)      j["totalEmails"] = *totalEmails;
        if (unreadEmails)     j["unreadEmails"] = *unreadEmails;
        if (totalThreads)     j["totalThreads"] = *totalThreads;
        if (unreadThreads)    j["unreadThreads"] = *unreadThreads;
        if (myRights)         j["myRights"] = myRights->toJson();
        j["isSubscribed"] = isSubscribed;

        return j;
    }
};

} // namespace aspose_jmap
