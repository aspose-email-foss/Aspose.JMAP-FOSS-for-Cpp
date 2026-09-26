#pragma once

#include "test_framework.hpp"
#include "../include/aspose_jmap/JmapClient.hpp"

#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <map>
#include <optional>
#include <stdexcept>

using namespace aspose_jmap;

namespace {

/// Simple transport that records the last request and returns a canned response.
/// Anonymous-namespaced: every test .cpp file that links into the same test binary
/// defines its own same-named FakeTransport with a different layout - without internal
/// linkage here, the linker treats them as one ODR-violating type (undefined behavior,
/// manifesting as random heap corruption at runtime).
class FakeTransport : public Transport {
public:
    HttpRequest lastRequest;
    HttpResponse cannedResponse;

    HttpResponse send(const HttpRequest& request) override {
        lastRequest = request;
        return cannedResponse;
    }
};

}  // namespace

/// Helper to build the expected Basic auth header for the given credentials.
static std::string buildExpectedAuth(const std::string& user, const std::string& pass) {
    // "user:pass" -> base64
    const std::string data = user + ":" + pass;
    static const char* tbl = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string out;
    out.reserve(((data.size() + 2) / 3) * 4);
    std::size_t i = 0;
    while (i < data.size()) {
        std::uint32_t a = i < data.size() ? static_cast<std::uint8_t>(data[i++]) : 0;
        std::uint32_t b = i < data.size() ? static_cast<std::uint8_t>(data[i++]) : 0;
        std::uint32_t c = i < data.size() ? static_cast<std::uint8_t>(data[i++]) : 0;
        std::uint32_t triple = (a << 16) + (b << 8) + c;
        out.push_back(tbl[(triple >> 18) & 0x3F]);
        out.push_back(tbl[(triple >> 12) & 0x3F]);
        out.push_back(tbl[(triple >> 6) & 0x3F]);
        out.push_back(tbl[triple & 0x3F]);
    }
    const std::size_t mod = data.size() % 3;
    if (mod) {
        out[out.size() - 1] = '=';
        if (mod == 1) {
            out[out.size() - 2] = '=';
        }
    }
    return "Basic " + out;
}

/// Minimal full Session JSON required by the client.
static nlohmann::json makeFullSessionJson(const std::string& baseUrl) {
    return nlohmann::json{
        {"capabilities", nlohmann::json::object()},
        {"accounts", nlohmann::json::object()},
        {"primaryAccounts", nlohmann::json::object()},
        {"username", "user@example.test"},
        {"apiUrl", baseUrl + "/api/"},
        {"downloadUrl", baseUrl + "/download/{accountId}/{blobId}/{type}/{name}"},
        {"uploadUrl", baseUrl + "/upload/{accountId}"},
        {"eventSourceUrl", baseUrl + "/events"},
        {"state", ""}
    };
}

AJ_TEST(Core_connectSuccess) {
    auto transport = std::make_shared<FakeTransport>();
    // Prepare session response.
    const std::string sessionUrl = "https://jmap.example.test/.well-known/jmap";
    transport->cannedResponse = {
        200,
        { {"Content-Type", "application/json"} },
        makeFullSessionJson("https://jmap.example.test").dump()
    };

    JmapClientOptions opts;
    opts.sessionUrl = sessionUrl;
    opts.username = "user";
    opts.password = "pass";
    opts.transport = transport;

    JmapClient client(opts);
    Session sess = client.connect();

    // Verify request.
    AJ_ASSERT_EQ(transport->lastRequest.method, "GET");
    AJ_ASSERT_EQ(transport->lastRequest.url, sessionUrl);
    AJ_ASSERT_TRUE(transport->lastRequest.headers.contains("Authorization"));
    AJ_ASSERT_EQ(transport->lastRequest.headers.at("Authorization"),
                 buildExpectedAuth("user", "pass"));

    // Verify session fields.
    AJ_ASSERT_EQ(sess.apiUrl, "https://jmap.example.test/api/");
    AJ_ASSERT_EQ(sess.uploadUrl, "https://jmap.example.test/upload/{accountId}");
    AJ_ASSERT_EQ(sess.downloadUrl,
                 "https://jmap.example.test/download/{accountId}/{blobId}/{type}/{name}");
}

AJ_TEST(Core_connectWithBearerToken) {
    // When bearerToken is set, it takes precedence over username/password: the
    // Authorization header must be "Bearer <token>" rather than Basic auth.
    auto transport = std::make_shared<FakeTransport>();
    const std::string sessionUrl = "https://jmap.example.test/.well-known/jmap";
    transport->cannedResponse = {
        200,
        { {"Content-Type", "application/json"} },
        makeFullSessionJson("https://jmap.example.test").dump()
    };

    JmapClientOptions opts;
    opts.sessionUrl = sessionUrl;
    opts.bearerToken = "eyJhbGciOi.example.token";
    opts.transport = transport;

    JmapClient client(opts);
    client.connect();

    AJ_ASSERT_TRUE(transport->lastRequest.headers.contains("Authorization"));
    AJ_ASSERT_EQ(transport->lastRequest.headers.at("Authorization"),
                 "Bearer eyJhbGciOi.example.token");
}

AJ_TEST(Core_echoSuccess) {
    auto transport = std::make_shared<FakeTransport>();
    // Session GET first.
    transport->cannedResponse = {
        200,
        { {"Content-Type", "application/json"} },
        makeFullSessionJson("https://jmap.example.test").dump()
    };
    JmapClientOptions opts;
    opts.sessionUrl = "https://jmap.example.test/.well-known/jmap";
    opts.username = "user";
    opts.password = "pass";
    opts.transport = transport;
    JmapClient client(opts);
    client.connect();

    // Prepare echo response.
    nlohmann::json echoArgs = { {"hello", true}, {"high", 5} };
    nlohmann::json respEnvelope = {
        {"methodResponses", nlohmann::json::array({
            nlohmann::json::array({ "Core/echo", echoArgs, "c1" })
        })},
        {"sessionState", "state1"}
    };
    transport->cannedResponse = {
        200,
        { {"Content-Type", "application/json"} },
        respEnvelope.dump()
    };

    // Call echo (public API defined by the final client).
    nlohmann::json result = client.echo(echoArgs);

    // Verify request details.
    AJ_ASSERT_EQ(transport->lastRequest.method, "POST");
    AJ_ASSERT_TRUE(transport->lastRequest.url.find("/api/") != std::string::npos);
    AJ_ASSERT_TRUE(transport->lastRequest.headers.contains("Authorization"));
    AJ_ASSERT_EQ(transport->lastRequest.headers.at("Authorization"),
                 buildExpectedAuth("user", "pass"));
    AJ_ASSERT_TRUE(transport->lastRequest.headers.contains("Content-Type"));
    AJ_ASSERT_EQ(transport->lastRequest.headers.at("Content-Type"), "application/json");

    // Parse request body to ensure correct invocation.
    nlohmann::json reqBody = nlohmann::json::parse(transport->lastRequest.body.value());
    AJ_ASSERT_TRUE(reqBody.contains("methodCalls"));
    AJ_ASSERT_EQ(reqBody["methodCalls"].size(), 1);
    AJ_ASSERT_EQ(reqBody["methodCalls"][0][0], "Core/echo");
    AJ_ASSERT_EQ(reqBody["methodCalls"][0][1], echoArgs);
    AJ_ASSERT_EQ(reqBody["methodCalls"][0][2], "c1");

    // Verify result matches echo arguments.
    AJ_ASSERT_EQ(result, echoArgs);
}

AJ_TEST(Core_echoProtocolError) {
    auto transport = std::make_shared<FakeTransport>();
    // Session GET.
    transport->cannedResponse = {
        200,
        { {"Content-Type", "application/json"} },
        makeFullSessionJson("https://jmap.example.test").dump()
    };
    JmapClientOptions opts;
    opts.sessionUrl = "https://jmap.example.test/.well-known/jmap";
    opts.username = "user";
    opts.password = "pass";
    opts.transport = transport;
    JmapClient client(opts);
    client.connect();

    // Prepare error response.
    nlohmann::json errorResp = {
        {"methodResponses", nlohmann::json::array({
            nlohmann::json::array({ "error",
                nlohmann::json{{"type", "unknownMethod"},
                               {"description", "Method not found"}},
                "c1" })
        })},
        {"sessionState", "state1"}
    };
    transport->cannedResponse = {
        200,
        { {"Content-Type", "application/json"} },
        errorResp.dump()
    };

    nlohmann::json dummyArgs = { {"foo", "bar"} };
    AJ_ASSERT_THROWS(client.echo(dummyArgs), JmapProtocolError);
}

AJ_TEST(Core_uploadBlobSuccess) {
    auto transport = std::make_shared<FakeTransport>();
    // Session GET.
    transport->cannedResponse = {
        200,
        { {"Content-Type", "application/json"} },
        makeFullSessionJson("https://jmap.example.test").dump()
    };
    JmapClientOptions opts;
    opts.sessionUrl = "https://jmap.example.test/.well-known/jmap";
    opts.username = "user";
    opts.password = "pass";
    opts.transport = transport;
    JmapClient client(opts);
    client.connect();

    // Prepare upload response.
    nlohmann::json uploadResp = {
        {"accountId", "A1"},
        {"blobId", "B123"},
        {"type", "text/plain"},
        {"size", 13}
    };
    transport->cannedResponse = {
        201,
        { {"Content-Type", "application/json"} },
        uploadResp.dump()
    };

    std::string content = "Hello, world!";
    nlohmann::json result = client.uploadBlob("A1", content, "text/plain");

    // Verify request.
    AJ_ASSERT_EQ(transport->lastRequest.method, "POST");
    AJ_ASSERT_TRUE(transport->lastRequest.url.find("/upload/A1") != std::string::npos);
    AJ_ASSERT_TRUE(transport->lastRequest.headers.contains("Authorization"));
    AJ_ASSERT_EQ(transport->lastRequest.headers.at("Authorization"),
                 buildExpectedAuth("user", "pass"));
    AJ_ASSERT_TRUE(transport->lastRequest.headers.contains("Content-Type"));
    AJ_ASSERT_EQ(transport->lastRequest.headers.at("Content-Type"), "text/plain");
    AJ_ASSERT_TRUE(transport->lastRequest.body.has_value());
    AJ_ASSERT_EQ(transport->lastRequest.body.value(), content);

    // Verify parsed JSON result.
    AJ_ASSERT_EQ(result["blobId"], "B123");
    AJ_ASSERT_EQ(result["type"], "text/plain");
    AJ_ASSERT_EQ(result["size"], 13);
}

AJ_TEST(Core_downloadBlobSuccess) {
    auto transport = std::make_shared<FakeTransport>();
    // Session GET.
    transport->cannedResponse = {
        200,
        { {"Content-Type", "application/json"} },
        makeFullSessionJson("https://jmap.example.test").dump()
    };
    JmapClientOptions opts;
    opts.sessionUrl = "https://jmap.example.test/.well-known/jmap";
    opts.username = "user";
    opts.password = "pass";
    opts.transport = transport;
    JmapClient client(opts);
    client.connect();

    // Prepare download response.
    const std::string blobData = "binarycontent";
    transport->cannedResponse = {
        200,
        { {"Content-Type", "application/octet-stream"} },
        blobData
    };

    std::string result = client.downloadBlob("A1", "B123", "application/octet-stream", std::nullopt);

    // Verify request.
    AJ_ASSERT_EQ(transport->lastRequest.method, "GET");
    AJ_ASSERT_TRUE(transport->lastRequest.url.find("/download/A1/B123/application%2Foctet-stream") != std::string::npos);
    AJ_ASSERT_TRUE(transport->lastRequest.headers.contains("Authorization"));
    AJ_ASSERT_EQ(transport->lastRequest.headers.at("Authorization"),
                 buildExpectedAuth("user", "pass"));
    AJ_ASSERT_FALSE(transport->lastRequest.body.has_value());

    // Verify returned body.
    AJ_ASSERT_EQ(result, blobData);
}

AJ_TEST(Core_uploadBlobPercentEncodesAccountId) {
    // Regression test: accountId is spliced verbatim into a URL path segment. A value
    // containing "/", "?", or "#" must be percent-encoded, or it would rewrite the request
    // path or smuggle extra query parameters into the upload URL.
    auto transport = std::make_shared<FakeTransport>();
    transport->cannedResponse = {
        200,
        { {"Content-Type", "application/json"} },
        makeFullSessionJson("https://jmap.example.test").dump()
    };
    JmapClientOptions opts;
    opts.sessionUrl = "https://jmap.example.test/.well-known/jmap";
    opts.username = "user";
    opts.password = "pass";
    opts.transport = transport;
    JmapClient client(opts);
    client.connect();

    nlohmann::json uploadResp = {
        {"accountId", "a/b"}, {"blobId", "b1"}, {"type", "text/plain"}, {"size", 1}
    };
    transport->cannedResponse = { 200, { {"Content-Type", "application/json"} }, uploadResp.dump() };

    client.uploadBlob("a/b", "x", "text/plain");

    AJ_ASSERT_EQ(transport->lastRequest.url, "https://jmap.example.test/upload/a%2Fb");
}
