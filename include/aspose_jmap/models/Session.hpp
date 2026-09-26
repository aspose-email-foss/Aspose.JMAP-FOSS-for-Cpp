#pragma once

#include <string>
#include <vector>
#include <map>
#include <cstdint>
#include <nlohmann/json.hpp>

#include "CommonTypes.hpp"

namespace aspose_jmap {

/// Represents the Core capability object found in Session.capabilities["urn:ietf:params:jmap:core"].
struct CoreCapability {
    std::uint64_t maxSizeUpload;
    std::uint64_t maxConcurrentUpload;
    std::uint64_t maxSizeRequest;
    std::uint64_t maxConcurrentRequests;
    std::uint64_t maxCallsInRequest;
    std::uint64_t maxObjectsInGet;
    std::uint64_t maxObjectsInSet;
    std::vector<std::string> collationAlgorithms;

    /// Construct from a JSON object, validating required fields.
    static CoreCapability fromJson(const nlohmann::json& data) {
        try {
            CoreCapability out;
            out.maxSizeUpload = data.at("maxSizeUpload").get<std::uint64_t>();
            out.maxConcurrentUpload = data.at("maxConcurrentUpload").get<std::uint64_t>();
            out.maxSizeRequest = data.at("maxSizeRequest").get<std::uint64_t>();
            out.maxConcurrentRequests = data.at("maxConcurrentRequests").get<std::uint64_t>();
            out.maxCallsInRequest = data.at("maxCallsInRequest").get<std::uint64_t>();
            out.maxObjectsInGet = data.at("maxObjectsInGet").get<std::uint64_t>();
            out.maxObjectsInSet = data.at("maxObjectsInSet").get<std::uint64_t>();
            out.collationAlgorithms = data.at("collationAlgorithms").get<std::vector<std::string>>();
            return out;
        } catch (const nlohmann::json::exception& e) {
            throw JmapProtocolError("CoreCapability parsing error: " + std::string(e.what()));
        }
    }

    /// Serialize to JSON.
    nlohmann::json toJson() const {
        return nlohmann::json{
            {"maxSizeUpload", maxSizeUpload},
            {"maxConcurrentUpload", maxConcurrentUpload},
            {"maxSizeRequest", maxSizeRequest},
            {"maxConcurrentRequests", maxConcurrentRequests},
            {"maxCallsInRequest", maxCallsInRequest},
            {"maxObjectsInGet", maxObjectsInGet},
            {"maxObjectsInSet", maxObjectsInSet},
            {"collationAlgorithms", collationAlgorithms}
        };
    }
};

/// Represents an account entry inside Session.accounts.
struct Account {
    std::string name;
    bool isPersonal;
    bool isReadOnly;
    std::map<std::string, nlohmann::json> accountCapabilities;

    /// Construct from a JSON object, validating required fields.
    static Account fromJson(const nlohmann::json& data) {
        try {
            Account out;
            out.name = data.at("name").get<std::string>();
            out.isPersonal = data.at("isPersonal").get<bool>();
            out.isReadOnly = data.at("isReadOnly").get<bool>();
            out.accountCapabilities = data.at("accountCapabilities")
                                          .get<std::map<std::string, nlohmann::json>>();
            return out;
        } catch (const nlohmann::json::exception& e) {
            throw JmapProtocolError("Account parsing error: " + std::string(e.what()));
        }
    }

    /// Serialize to JSON.
    nlohmann::json toJson() const {
        return nlohmann::json{
            {"name", name},
            {"isPersonal", isPersonal},
            {"isReadOnly", isReadOnly},
            {"accountCapabilities", accountCapabilities}
        };
    }
};

/// The JMAP Session resource. Fetched once (GET, no body) from a well‑known URL and cached; describes
/// server capabilities, accounts, and the URL templates used for every subsequent request.
struct Session {
    std::map<std::string, nlohmann::json> capabilities;
    std::map<std::string, Account> accounts;
    std::map<std::string, std::string> primaryAccounts;
    std::string username;
    std::string apiUrl;
    std::string downloadUrl;
    std::string uploadUrl;
    std::string eventSourceUrl;
    std::string state;

    /// Construct from a JSON object, validating required fields.
    static Session fromJson(const nlohmann::json& data) {
        try {
            Session out;
            out.capabilities = data.at("capabilities")
                                   .get<std::map<std::string, nlohmann::json>>();

            // accounts: map<Id, Account>
            const auto& accountsJson = data.at("accounts");
            for (auto it = accountsJson.begin(); it != accountsJson.end(); ++it) {
                out.accounts.emplace(it.key(), Account::fromJson(it.value()));
            }

            out.primaryAccounts = data.at("primaryAccounts")
                                      .get<std::map<std::string, std::string>>();
            out.username = data.at("username").get<std::string>();
            out.apiUrl = data.at("apiUrl").get<std::string>();
            out.downloadUrl = data.at("downloadUrl").get<std::string>();
            out.uploadUrl = data.at("uploadUrl").get<std::string>();
            out.eventSourceUrl = data.at("eventSourceUrl").get<std::string>();
            out.state = data.at("state").get<std::string>();
            return out;
        } catch (const nlohmann::json::exception& e) {
            throw JmapProtocolError("Session parsing error: " + std::string(e.what()));
        }
    }

    /// Serialize to JSON.
    nlohmann::json toJson() const {
        nlohmann::json accountsJson = nlohmann::json::object();
        for (const auto& [id, acc] : accounts) {
            accountsJson[id] = acc.toJson();
        }

        return nlohmann::json{
            {"capabilities", capabilities},
            {"accounts", accountsJson},
            {"primaryAccounts", primaryAccounts},
            {"username", username},
            {"apiUrl", apiUrl},
            {"downloadUrl", downloadUrl},
            {"uploadUrl", uploadUrl},
            {"eventSourceUrl", eventSourceUrl},
            {"state", state}
        };
    }
};

} // namespace aspose_jmap
