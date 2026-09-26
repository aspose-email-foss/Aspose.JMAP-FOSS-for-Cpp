#include "../test_framework.hpp"
#include "../../include/aspose_jmap/models/Transport.hpp"

using namespace aspose_jmap;

namespace {

/// Simple fake transport that records the request it receives and returns a preset response.
/// Anonymous-namespaced: every test .cpp file that links into the same test binary
/// defines its own same-named FakeTransport with a different layout - without internal
/// linkage here, the linker treats them as one ODR-violating type (undefined behavior,
/// manifesting as random heap corruption).
class FakeTransport : public Transport {
public:
    HttpResponse send(const HttpRequest& request) override {
        capturedRequest = request;
        return presetResponse;
    }

    HttpRequest capturedRequest;
    HttpResponse presetResponse;
};

}  // namespace

AJ_TEST(Transport_FakeSend_NoBody) {
    // Arrange: build a request without a body and a canned response.
    HttpRequest req;
    req.method = "GET";
    req.url = "http://example.com/api";
    req.headers = { {"Accept", "application/json"}, {"User-Agent", "test"} };
    // req.body remains std::nullopt

    HttpResponse resp;
    resp.statusCode = 200;
    resp.headers = { {"Content-Type", "application/json"} };
    resp.body = R"({"ok":true})";

    FakeTransport transport;
    transport.presetResponse = resp;

    // Act: send the request via the fake transport.
    HttpResponse actual = transport.send(req);

    // Assert: the returned response matches the preset one.
    AJ_ASSERT_EQ(actual.statusCode, resp.statusCode);
    AJ_ASSERT_EQ(actual.body, resp.body);
    AJ_ASSERT_EQ(actual.headers.size(), resp.headers.size());
    AJ_ASSERT_EQ(actual.headers.at("Content-Type"), resp.headers.at("Content-Type"));

    // Assert: the transport recorded the request correctly.
    AJ_ASSERT_EQ(transport.capturedRequest.method, req.method);
    AJ_ASSERT_EQ(transport.capturedRequest.url, req.url);
    AJ_ASSERT_EQ(transport.capturedRequest.headers.size(), req.headers.size());
    AJ_ASSERT_EQ(transport.capturedRequest.headers.at("Accept"), req.headers.at("Accept"));
    AJ_ASSERT_TRUE(!transport.capturedRequest.body.has_value());
}

AJ_TEST(Transport_FakeSend_WithBody) {
    // Arrange: request with a body and empty headers.
    HttpRequest req;
    req.method = "POST";
    req.url = "https://example.org/submit";
    req.headers = {}; // empty
    req.body = std::string(R"({"name":"test"})");

    HttpResponse resp;
    resp.statusCode = 201;
    resp.headers = { {"Location", "/resource/123"} };
    resp.body = ""; // empty body

    FakeTransport transport;
    transport.presetResponse = resp;

    // Act
    HttpResponse actual = transport.send(req);

    // Assert response
    AJ_ASSERT_EQ(actual.statusCode, 201);
    AJ_ASSERT_EQ(actual.headers.at("Location"), "/resource/123");
    AJ_ASSERT_TRUE(actual.body.empty());

    // Assert request captured
    AJ_ASSERT_EQ(transport.capturedRequest.method, "POST");
    AJ_ASSERT_EQ(transport.capturedRequest.url, "https://example.org/submit");
    AJ_ASSERT_TRUE(transport.capturedRequest.headers.empty());
    AJ_ASSERT_TRUE(transport.capturedRequest.body.has_value());
    AJ_ASSERT_EQ(*transport.capturedRequest.body, R"({"name":"test"})");
}
