#pragma once

#include "models/Comparator.hpp"
#include "models/EmailSubmission.hpp"
#include "models/CommonTypes.hpp"
#include "models/Invocation.hpp"
#include "client_core.hpp"

#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <optional>
#include <map>
#include <utility>

namespace aspose_jmap {

/// Mixin providing the JMAP Submission methods.
class SubmissionClientMixin : public virtual JmapClientCore {
public:
    /// Forwards to JmapClientCore's constructor. Only relevant when SubmissionClientMixin
    /// is instantiated on its own (e.g. in a test); when composed into JmapClient, the
    /// virtual base is actually initialized by JmapClient's own constructor instead, per
    /// C++ virtual-inheritance rules - this override exists purely so the compiler doesn't
    /// implicitly delete SubmissionClientMixin's default constructor.
    explicit SubmissionClientMixin(JmapClientOptions options) : JmapClientCore(std::move(options)) {}

    /// Send (create) an EmailSubmission.
    /// @param account_id Account identifier.
    /// @param submission EmailSubmission object containing the required fields.
    /// @return EmailSubmission populated with server‑assigned fields.
    /// @throws JmapNetworkError on transport failure.
    /// @throws JmapProtocolError on protocol‑level errors or malformed responses.
    inline EmailSubmission send(const std::string& account_id,
                                const EmailSubmission& submission) {
        // Build arguments for EmailSubmission/set (create only).
        nlohmann::json createMap = nlohmann::json::object();
        createMap["c1"] = submission.toJson();

        std::map<std::string, nlohmann::json> args;
        args["accountId"] = account_id;
        args["create"] = createMap;

        std::vector<Invocation> calls = {
            Invocation{ "EmailSubmission/set", std::move(args), "c1" }
        };

        nlohmann::json resp = this->sendRequest(calls,
                                                { "urn:ietf:params:jmap:submission" });

        // Extract the first method response.
        const auto& methodResponses = resp.at("methodResponses");
        const auto& firstResponse = methodResponses.at(0);
        const auto& respArgs = firstResponse.at(1);

        // Server‑provided partial object under "created".
        const auto& created = respArgs.at("created");
        const auto& serverObj = created.at("c1");

        // Merge server fields over the original submission.
        nlohmann::json merged = submission.toJson();
        for (auto& [k, v] : serverObj.items()) {
            merged[k] = v;
        }

        return EmailSubmission::fromJson(merged);
    }

    /// Cancel a previously sent EmailSubmission.
    /// @param account_id Account identifier.
    /// @param submission_id Identifier of the EmailSubmission to cancel.
    /// @throws JmapNetworkError on transport failure.
    /// @throws JmapProtocolError on protocol‑level errors.
    inline void cancelSend(const std::string& account_id,
                           const std::string& submission_id) {
        // Build a patch object that sets undoStatus to "canceled". A PatchObject key is a
        // JSON Pointer (RFC 6901) relative to the object being patched - a bare top-level
        // property name has no leading slash; "/undoStatus" would instead point at a
        // property literally named the empty string, which a real JMAP server rejects/ignores.
        nlohmann::json patch = nlohmann::json::object();
        patch["undoStatus"] = "canceled";

        nlohmann::json updateMap = nlohmann::json::object();
        updateMap[submission_id] = patch;

        std::map<std::string, nlohmann::json> args;
        args["accountId"] = account_id;
        args["update"] = updateMap;

        std::vector<Invocation> calls = {
            Invocation{ "EmailSubmission/set", std::move(args), "c1" }
        };

        // We ignore the response content; any error will be thrown by sendRequest.
        this->sendRequest(calls, { "urn:ietf:params:jmap:submission" });
    }

    /// List EmailSubmission objects for an account.
    /// @param account_id Account identifier.
    /// @param ids Optional list of specific submission ids to retrieve; if omitted, all are returned.
    /// @param properties Optional list of properties to include; if omitted, all are returned.
    /// @return Vector of EmailSubmission objects.
    /// @throws JmapNetworkError on transport failure.
    /// @throws JmapProtocolError on protocol‑level errors or malformed responses.
    inline std::vector<EmailSubmission> listSubmissions(
        const std::string& account_id,
        const std::optional<std::vector<std::string>>& ids = std::nullopt,
        const std::optional<std::vector<std::string>>& properties = std::nullopt) {

        std::map<std::string, nlohmann::json> args;
        args["accountId"] = account_id;
        if (ids) {
            args["ids"] = *ids;
        }
        if (properties) {
            args["properties"] = *properties;
        }

        std::vector<Invocation> calls = {
            Invocation{ "EmailSubmission/get", std::move(args), "c1" }
        };

        nlohmann::json resp = this->sendRequest(calls,
                                                { "urn:ietf:params:jmap:submission" });

        const auto& methodResponses = resp.at("methodResponses");
        const auto& firstResponse = methodResponses.at(0);
        const auto& respArgs = firstResponse.at(1);

        const auto& list = respArgs.at("list");
        std::vector<EmailSubmission> result;
        result.reserve(list.size());

        for (const auto& item : list) {
            result.emplace_back(EmailSubmission::fromJson(item));
        }

        return result;
    }
};

} // namespace aspose_jmap
