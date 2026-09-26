#pragma once

#include <string>
#include <vector>
#include <map>
#include <optional>
#include <cstdint>
#include <utility>
#include <nlohmann/json.hpp>

#include "models/Comparator.hpp"
#include "models/Email.hpp"
#include "models/Identity.hpp"
#include "models/Mailbox.hpp"
#include "models/SearchSnippet.hpp"
#include "models/EmailQueryResponse.hpp"
#include "models/Thread.hpp"
#include "models/CommonTypes.hpp"
#include "models/Invocation.hpp"
#include "client_core.hpp"

namespace aspose_jmap {

/// Mixin providing the JMAP *mail* capability methods.
class MailClientMixin : public virtual JmapClientCore {
public:
    /// Forwards to JmapClientCore's constructor. Only relevant when MailClientMixin is
    /// instantiated on its own (e.g. in a test); when composed into JmapClient, the
    /// virtual base is actually initialized by JmapClient's own constructor instead, per
    /// C++ virtual-inheritance rules - this override exists purely so the compiler doesn't
    /// implicitly delete MailClientMixin's default constructor (it can't default-construct
    /// a virtual base that has no default constructor of its own).
    explicit MailClientMixin(JmapClientOptions options) : JmapClientCore(std::move(options)) {}

    /// List all mailboxes for the given account.
    /// @throws JmapProtocolError / JmapNetworkError on failure.
    inline std::vector<Mailbox> listMailboxes(const std::string& accountId) {
        std::map<std::string, nlohmann::json> args{
            {"accountId", accountId}
        };
        Invocation inv{ "Mailbox/get", std::move(args), "c1" };
        nlohmann::json resp = sendRequest({inv}, {"urn:ietf:params:jmap:mail"});
        auto invResp = Invocation::fromJson(resp.at("methodResponses").at(0));
        const auto& list = invResp.arguments.at("list");
        std::vector<Mailbox> out;
        out.reserve(list.size());
        for (const auto& item : list) {
            out.emplace_back(Mailbox::fromJson(item));
        }
        return out;
    }

    /// Retrieve specific mailboxes by id.
    /// @param ids Optional list of mailbox ids to fetch; if omitted all are returned.
    /// @param properties Optional list of properties to include.
    inline std::vector<Mailbox> getMailbox(const std::string& accountId,
                                           const std::optional<std::vector<std::string>>& ids = std::nullopt,
                                           const std::optional<std::vector<std::string>>& properties = std::nullopt) {
        std::map<std::string, nlohmann::json> args{
            {"accountId", accountId}
        };
        if (ids) args["ids"] = *ids;
        if (properties) args["properties"] = *properties;

        Invocation inv{ "Mailbox/get", std::move(args), "c1" };
        nlohmann::json resp = sendRequest({inv}, {"urn:ietf:params:jmap:mail"});
        auto invResp = Invocation::fromJson(resp.at("methodResponses").at(0));
        const auto& list = invResp.arguments.at("list");
        std::vector<Mailbox> out;
        out.reserve(list.size());
        for (const auto& item : list) {
            out.emplace_back(Mailbox::fromJson(item));
        }
        return out;
    }

    /// Create a new mailbox. The server‑assigned fields are merged onto the supplied object.
    inline Mailbox createMailbox(const std::string& accountId, const Mailbox& mailbox) {
        // client‑generated id for the create map
        const std::string clientId = "new";

        std::map<std::string, nlohmann::json> createMap{
            {clientId, mailbox.toJson()}
        };
        std::map<std::string, nlohmann::json> args{
            {"accountId", accountId},
            {"create", std::move(createMap)}
        };
        Invocation inv{ "Mailbox/set", std::move(args), "c1" };
        nlohmann::json resp = sendRequest({inv}, {"urn:ietf:params:jmap:mail"});
        auto invResp = Invocation::fromJson(resp.at("methodResponses").at(0));
        const auto& created = invResp.arguments.at("created");
        if (!created.contains(clientId)) {
            throw JmapProtocolError("Mailbox/set response missing 'created' entry for client id");
        }
        nlohmann::json merged = mailbox.toJson();
        for (auto& [k, v] : created.at(clientId).items()) {
            merged[k] = v;
        }
        return Mailbox::fromJson(merged);
    }

    /// Delete mailboxes by id.
    /// @return List of ids that were successfully destroyed.
    inline std::vector<std::string> deleteMailbox(const std::string& accountId,
                                                  const std::vector<std::string>& ids) {
        std::map<std::string, nlohmann::json> args{
            {"accountId", accountId},
            {"destroy", ids}
        };
        Invocation inv{ "Mailbox/set", std::move(args), "c1" };
        nlohmann::json resp = sendRequest({inv}, {"urn:ietf:params:jmap:mail"});
        auto invResp = Invocation::fromJson(resp.at("methodResponses").at(0));
        const auto& destroyed = invResp.arguments.at("destroyed");
        std::vector<std::string> out;
        out.reserve(destroyed.size());
        for (const auto& id : destroyed) {
            out.emplace_back(id.get<std::string>());
        }
        return out;
    }

    /// Query message ids matching an optional filter.
    /// @param filter Optional filter object (as raw JSON) as defined by JMAP.
    /// @param sort Optional sort comparators.
    /// @param position The zero-based index of the first result to return. Defaults to 0.
    /// @param limit Optional maximum number of ids to return.
    /// @return The full JMAP `Email/query` response (RFC 8620 section 5.5).
    inline EmailQueryResponse listMessages(const std::string& accountId,
                                           const std::optional<nlohmann::json>& filter = std::nullopt,
                                           const std::optional<std::vector<Comparator>>& sort = std::nullopt,
                                           std::uint64_t position = 0,
                                           const std::optional<std::uint64_t>& limit = std::nullopt) {
        std::map<std::string, nlohmann::json> args{
            {"accountId", accountId},
            {"position", position}
        };
        if (filter) args["filter"] = *filter;
        if (sort) {
            nlohmann::json arr = nlohmann::json::array();
            for (const auto& comp : *sort) arr.push_back(comp.toJson());
            args["sort"] = std::move(arr);
        }
        if (limit) args["limit"] = *limit;

        Invocation inv{ "Email/query", std::move(args), "c1" };
        nlohmann::json resp = sendRequest({inv}, {"urn:ietf:params:jmap:mail"});
        auto invResp = Invocation::fromJson(resp.at("methodResponses").at(0));
        return EmailQueryResponse::fromJson(invResp.arguments);
    }

    /// Fetch full Email objects for the given ids.
    inline std::vector<Email> fetchMessage(const std::string& accountId,
                                           const std::vector<std::string>& ids,
                                           const std::optional<std::vector<std::string>>& properties = std::nullopt) {
        std::map<std::string, nlohmann::json> args{
            {"accountId", accountId},
            {"ids", ids}
        };
        if (properties) args["properties"] = *properties;

        Invocation inv{ "Email/get", std::move(args), "c1" };
        nlohmann::json resp = sendRequest({inv}, {"urn:ietf:params:jmap:mail"});
        auto invResp = Invocation::fromJson(resp.at("methodResponses").at(0));
        const auto& list = invResp.arguments.at("list");
        std::vector<Email> out;
        out.reserve(list.size());
        for (const auto& item : list) {
            out.emplace_back(Email::fromJson(item));
        }
        return out;
    }

    /// Move a single email to a different mailbox.
    /// @return The server‑provided partial update object.
    inline nlohmann::json moveMessage(const std::string& accountId,
                                      const std::string& emailId,
                                      const std::string& destMailboxId) {
        nlohmann::json updateObj = {
            {"mailboxIds", {{destMailboxId, true}}}
        };
        std::map<std::string, nlohmann::json> updateMap{
            {emailId, updateObj}
        };
        std::map<std::string, nlohmann::json> args{
            {"accountId", accountId},
            {"update", std::move(updateMap)}
        };
        Invocation inv{ "Email/set", std::move(args), "c1" };
        nlohmann::json resp = sendRequest({inv}, {"urn:ietf:params:jmap:mail"});
        auto invResp = Invocation::fromJson(resp.at("methodResponses").at(0));
        return invResp.arguments;
    }

    /// Set keywords on one or more messages.
    /// @param keywords Map of keyword name to boolean value.
    /// @return The server‑provided partial update object.
    inline nlohmann::json setMessageKeyword(const std::string& accountId,
                                            const std::vector<std::string>& emailIds,
                                            const std::map<std::string, bool>& keywords) {
        std::map<std::string, nlohmann::json> updateMap;
        for (const auto& id : emailIds) {
            updateMap[id] = nlohmann::json{{"keywords", keywords}};
        }
        std::map<std::string, nlohmann::json> args{
            {"accountId", accountId},
            {"update", std::move(updateMap)}
        };
        Invocation inv{ "Email/set", std::move(args), "c1" };
        nlohmann::json resp = sendRequest({inv}, {"urn:ietf:params:jmap:mail"});
        auto invResp = Invocation::fromJson(resp.at("methodResponses").at(0));
        return invResp.arguments.at("updated");
    }

    /// Delete messages by id.
    /// @return List of ids that were successfully destroyed.
    inline std::vector<std::string> deleteMessage(const std::string& accountId,
                                                  const std::vector<std::string>& ids) {
        std::map<std::string, nlohmann::json> args{
            {"accountId", accountId},
            {"destroy", ids}
        };
        Invocation inv{ "Email/set", std::move(args), "c1" };
        nlohmann::json resp = sendRequest({inv}, {"urn:ietf:params:jmap:mail"});
        auto invResp = Invocation::fromJson(resp.at("methodResponses").at(0));
        const auto& destroyed = invResp.arguments.at("destroyed");
        std::vector<std::string> out;
        out.reserve(destroyed.size());
        for (const auto& id : destroyed) {
            out.emplace_back(id.get<std::string>());
        }
        return out;
    }

    /// List identities for the given account.
    inline std::vector<Identity> listIdentities(const std::string& accountId) {
        std::map<std::string, nlohmann::json> args{
            {"accountId", accountId}
        };
        Invocation inv{ "Identity/get", std::move(args), "c1" };
        nlohmann::json resp = sendRequest({inv}, {"urn:ietf:params:jmap:mail"});
        auto invResp = Invocation::fromJson(resp.at("methodResponses").at(0));
        const auto& list = invResp.arguments.at("list");
        std::vector<Identity> out;
        out.reserve(list.size());
        for (const auto& item : list) {
            out.emplace_back(Identity::fromJson(item));
        }
        return out;
    }
};

} // namespace aspose_jmap
