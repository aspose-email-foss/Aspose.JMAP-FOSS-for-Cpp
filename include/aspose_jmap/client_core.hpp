#pragma once

#include "models/Invocation.hpp"
#include "models/CommonTypes.hpp"
#include "models/JmapRequestEnvelope.hpp"
#include "models/JmapResponseEnvelope.hpp"
#include "models/Session.hpp"
#include "models/Transport.hpp"

#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <map>
#include <optional>
#include <memory>
#include <stdexcept>
#include <cstdint>
#include <cctype>

namespace aspose_jmap {

/// Options used to configure a JMAP client.
struct JmapClientOptions {
    /// URL of the JMAP session resource (e.g. "https://example.com/.well-known/jmap").
    std::string sessionUrl;

    /// Username for HTTP Basic authentication.
    std::string username;

    /// Password for HTTP Basic authentication.
    std::string password;

    /// Optional OAuth 2.0 bearer token (RFC 6750). When set to a non-empty value, it takes
    /// precedence over `username`/`password`: the client authenticates with
    /// "Authorization: Bearer <bearerToken>" instead of HTTP Basic auth.
    std::optional<std::string> bearerToken;

    /// Optional transport implementation. If null, a default WinHttpTransport is created
    /// (Windows only - on other platforms, leaving this null throws; supply a custom
    /// Transport subclass instead).
    std::shared_ptr<Transport> transport = nullptr;
};

/// Core client providing session handling, request sending and blob utilities.
class JmapClientCore {
protected:
    std::shared_ptr<Transport> transport_;   ///< HTTP transport used for all calls.
    std::string authHeader_;                 ///< Cached "Authorization: Basic ..." or "Authorization: Bearer ..." header.
    Session session_;                        ///< Last fetched session object.
    std::string apiUrl_;                     ///< Resolved API URL from the session.
    std::string uploadUrl_;                  ///< Resolved upload URL from the session.
    std::string downloadUrl_;                ///< Resolved download URL from the session.
    std::string sessionUrl_;                 ///< Original session URL supplied by the user.

    // ------------------------------------------------------------------------
    // Helper: Base64 encoding (RFC 4648, no padding removal needed for Basic auth).
    // ------------------------------------------------------------------------
    static std::string base64Encode(const std::string& data) {
        static const char* tbl = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        std::string out;
        out.reserve(((data.size() + 2) / 3) * 4);
        std::size_t i = 0;
        while (i < data.size()) {
            std::uint32_t octet_a = i < data.size() ? static_cast<std::uint8_t>(data[i++]) : 0;
            std::uint32_t octet_b = i < data.size() ? static_cast<std::uint8_t>(data[i++]) : 0;
            std::uint32_t octet_c = i < data.size() ? static_cast<std::uint8_t>(data[i++]) : 0;

            std::uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

            out.push_back(tbl[(triple >> 3 * 6) & 0x3F]);
            out.push_back(tbl[(triple >> 2 * 6) & 0x3F]);
            out.push_back(tbl[(triple >> 1 * 6) & 0x3F]);
            out.push_back(tbl[(triple >> 0 * 6) & 0x3F]);
        }

        const std::size_t mod = data.size() % 3;
        if (mod) {
            out[out.size() - 1] = '=';
            if (mod == 1) {
                out[out.size() - 2] = '=';
            }
        }
        return out;
    }

    // ------------------------------------------------------------------------
    // Helper: Build the Authorization header once.
    // ------------------------------------------------------------------------
    static std::string buildAuthHeader(const std::string& user, const std::string& pass) {
        return "Basic " + base64Encode(user + ":" + pass);
    }

    // ------------------------------------------------------------------------
    // Helper: Resolve the Authorization header from the supplied options. Prefers an
    // OAuth 2.0 bearer token (RFC 6750) when one is present and non-empty, falling back
    // to HTTP Basic auth built from username/password otherwise.
    // ------------------------------------------------------------------------
    static std::string resolveAuthHeader(const JmapClientOptions& options) {
        if (options.bearerToken && !options.bearerToken->empty()) {
            return "Bearer " + *options.bearerToken;
        }
        return buildAuthHeader(options.username, options.password);
    }

    // ------------------------------------------------------------------------
    // Helper: Resolve a possibly‑relative URL against a base URL.
    // ------------------------------------------------------------------------
    static std::string resolveUrl(const std::string& base, const std::string& url) {
        if (url.rfind("http://", 0) == 0 || url.rfind("https://", 0) == 0) {
            return url; // already absolute
        }
        // Extract scheme + authority from base (up to first '/' after "://").
        std::size_t schemePos = base.find("://");
        if (schemePos == std::string::npos) {
            throw JmapProtocolError("Invalid base URL: " + base);
        }
        std::size_t afterScheme = schemePos + 3;
        std::size_t pathPos = base.find('/', afterScheme);
        std::string origin = (pathPos == std::string::npos) ? base : base.substr(0, pathPos);
        if (url.empty() || url[0] != '/') {
            // treat as relative path segment (unlikely for JMAP URLs)
            return origin + '/' + url;
        }
        return origin + url;
    }

    // ------------------------------------------------------------------------
    // Helper: percent-encode a value substituted into a URL path segment. `/`, `?`, `#`,
    // etc. in an un-encoded value would otherwise rewrite the request path or smuggle
    // extra query parameters into the request. RFC 3986 unreserved characters (beyond
    // alphanumerics: '-', '.', '_', '~') are left as-is, matching the other language
    // targets' URL-encoding behavior.
    // ------------------------------------------------------------------------
    static std::string percentEncode(const std::string& value) {
        static const char* hex = "0123456789ABCDEF";
        std::string result;
        result.reserve(value.size());
        for (unsigned char c : value) {
            if (std::isalnum(c) || c == '-' || c == '.' || c == '_' || c == '~') {
                result.push_back(static_cast<char>(c));
            } else {
                result.push_back('%');
                result.push_back(hex[c >> 4]);
                result.push_back(hex[c & 0x0F]);
            }
        }
        return result;
    }

    // ------------------------------------------------------------------------
    // Helper: Replace placeholders like {accountId} in a URL, percent-encoding each
    // substituted value.
    // ------------------------------------------------------------------------
    static std::string replacePlaceholders(const std::string& url,
                                           const std::map<std::string, std::string>& values) {
        std::string result = url;
        for (const auto& [key, rawVal] : values) {
            std::string val = percentEncode(rawVal);
            std::string placeholder = "{" + key + "}";
            std::size_t pos = 0;
            while ((pos = result.find(placeholder, pos)) != std::string::npos) {
                result.replace(pos, placeholder.size(), val);
                pos += val.size();
            }
        }
        return result;
    }

    // ------------------------------------------------------------------------
    // Helper: the transport used when JmapClientOptions::transport is left null.
    // WinHttpTransport only exists on Windows (see models/Transport.hpp) - on other
    // platforms there is no built-in default, so fail clearly at construction time
    // rather than failing to compile the whole library.
    // ------------------------------------------------------------------------
    static std::shared_ptr<Transport> defaultTransport() {
#ifdef _WIN32
        return std::make_shared<WinHttpTransport>();
#else
        throw std::runtime_error(
            "aspose_jmap: no Transport supplied and no default transport is available on "
            "this platform (WinHttpTransport is Windows-only); construct JmapClientOptions "
            "with a custom Transport subclass");
#endif
    }

public:
    /// Constructs a core client with the given options.
    explicit JmapClientCore(JmapClientOptions options)
        : transport_(options.transport ? options.transport : defaultTransport()),
          authHeader_(resolveAuthHeader(options)),
          sessionUrl_(std::move(options.sessionUrl)) {}

    /// Retrieves the JMAP session object and resolves capability URLs.
    /// @throws JmapNetworkError on transport failure, JmapProtocolError on malformed JSON.
    Session connect() {
        HttpRequest req;
        req.method = "GET";
        req.url = sessionUrl_;
        req.headers = { {"Authorization", authHeader_} };
        req.body = std::nullopt;

        HttpResponse resp = transport_->send(req);
        if (resp.statusCode != 200) {
            throw JmapNetworkError("Failed to fetch session: HTTP " + std::to_string(resp.statusCode));
        }

        nlohmann::json json = nlohmann::json::parse(resp.body);
        session_ = Session::fromJson(json);

        // Resolve possibly‑relative URLs against the session URL's origin.
        apiUrl_ = resolveUrl(sessionUrl_, session_.apiUrl);
        uploadUrl_ = resolveUrl(sessionUrl_, session_.uploadUrl);
        downloadUrl_ = resolveUrl(sessionUrl_, session_.downloadUrl);

        return session_;
    }

    /// Sends a JMAP request envelope containing one or more method calls.
    /// @param methodCalls List of method invocations to send.
    /// @param usingCapabilities List of capability URNs required for the calls.
    /// @return Parsed JSON response envelope.
    /// @throws JmapProtocolError if any response entry is an error.
    /// @throws JmapNetworkError on transport‑level failures.
    inline nlohmann::json sendRequest(const std::vector<Invocation>& methodCalls,
                                      const std::vector<std::string>& usingCapabilities) {
        JmapRequestEnvelope envelope;
        envelope.using_ = usingCapabilities;
        envelope.methodCalls = methodCalls;

        std::string bodyStr = envelope.toJson().dump();

        HttpRequest req;
        req.method = "POST";
        req.url = apiUrl_;
        req.headers = {
            {"Authorization", authHeader_},
            {"Content-Type", "application/json"}
        };
        req.body = bodyStr;

        HttpResponse resp = transport_->send(req);
        if (resp.statusCode != 200) {
            throw JmapNetworkError("JMAP request failed: HTTP " + std::to_string(resp.statusCode));
        }

        nlohmann::json respJson = nlohmann::json::parse(resp.body);
        JmapResponseEnvelope responseEnvelope = JmapResponseEnvelope::fromJson(respJson);

        for (const auto& inv : responseEnvelope.methodResponses) {
            if (inv.name == "error") {
                std::string type = inv.arguments.value("type", "");
                std::string description = inv.arguments.value("description", "");
                throw JmapProtocolError(type, description);
            }
        }
        return respJson;
    }

    /// Calls Core/echo, returning the server-echoed arguments verbatim.
    /// @param args Arbitrary JSON object to echo.
    /// @throws JmapProtocolError / JmapNetworkError on failure.
    inline nlohmann::json echo(const nlohmann::json& args) {
        Invocation inv{"Core/echo", args, "c1"};
        nlohmann::json resp = sendRequest({inv}, {"urn:ietf:params:jmap:core"});
        const auto& responses = resp.at("methodResponses");
        if (responses.empty()) {
            throw JmapProtocolError("Core/echo response missing methodResponses entry");
        }
        return responses.at(0).at(1);
    }

    /// Uploads a binary blob to the server.
    /// @param accountId Account identifier to substitute into the upload URL.
    /// @param content Raw bytes to upload.
    /// @param contentType MIME type of the content.
    /// @return JSON object containing at least {accountId, blobId, type, size}.
    /// @throws JmapNetworkError on transport failure or non‑2xx status.
    inline nlohmann::json uploadBlob(const std::string& accountId,
                                     const std::string& content,
                                     const std::string& contentType) {
        std::string url = replacePlaceholders(uploadUrl_, { {"accountId", accountId} });

        HttpRequest req;
        req.method = "POST";
        req.url = url;
        req.headers = {
            {"Authorization", authHeader_},
            {"Content-Type", contentType}
        };
        req.body = content;

        HttpResponse resp = transport_->send(req);
        if (resp.statusCode < 200 || resp.statusCode >= 300) {
            throw JmapNetworkError("UploadBlob failed: HTTP " + std::to_string(resp.statusCode));
        }

        return nlohmann::json::parse(resp.body);
    }

    /// Downloads a previously uploaded blob.
    /// @param accountId Account identifier.
    /// @param blobId Identifier of the blob to download.
    /// @param type MIME type of the blob (used for URL expansion).
    /// @param name Optional filename placeholder for the URL.
    /// @return Raw bytes of the blob.
    /// @throws JmapNetworkError on transport failure or non‑2xx status.
    inline std::string downloadBlob(const std::string& accountId,
                                    const std::string& blobId,
                                    const std::string& type,
                                    const std::optional<std::string>& name = std::nullopt) {
        std::map<std::string, std::string> vals = {
            {"accountId", accountId},
            {"blobId", blobId},
            {"type", type}
        };
        if (name) {
            vals.emplace("name", *name);
        }
        std::string url = replacePlaceholders(downloadUrl_, vals);

        HttpRequest req;
        req.method = "GET";
        req.url = url;
        req.headers = { {"Authorization", authHeader_} };
        req.body = std::nullopt;

        HttpResponse resp = transport_->send(req);
        if (resp.statusCode != 200) {
            throw JmapNetworkError("DownloadBlob failed: HTTP " + std::to_string(resp.statusCode));
        }
        return resp.body;
    }
};

} // namespace aspose_jmap
