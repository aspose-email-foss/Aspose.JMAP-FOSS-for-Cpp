#pragma once

#include "test_framework.hpp"
#include "../include/aspose_jmap/JmapClient.hpp"

#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <optional>
#include <queue>
#include <memory>

using namespace aspose_jmap;

namespace {

/// Simple fake transport that records the last request and returns queued responses.
/// Anonymous-namespaced: every test .cpp file that links into the same test binary
/// defines its own same-named FakeTransport with a different layout - without internal
/// linkage here, the linker treats them as one ODR-violating type (undefined behavior,
/// manifesting as random heap corruption).
class FakeTransport : public Transport {
public:
    HttpRequest lastRequest;
    std::queue<HttpResponse> responses;

    // Enqueue a response that will be returned on the next send().
    void enqueueResponse(const HttpResponse& resp) { responses.push(resp); }

    HttpResponse send(const HttpRequest& request) override {
        lastRequest = request;
        if (responses.empty()) {
            throw JmapNetworkError("No fake response queued");
        }
        HttpResponse resp = responses.front();
        responses.pop();
        return resp;
    }
};

}  // namespace

/// Helper to build a minimal Session JSON required by JmapClientCore::connect().
static std::string makeSessionJson(const std::string& apiUrl) {
    nlohmann::json sess;
    sess["capabilities"] = nlohmann::json::object();
    sess["accounts"] = nlohmann::json::object();
    sess["primaryAccounts"] = nlohmann::json::object();
    sess["username"] = "testuser";
    sess["apiUrl"] = apiUrl;
    sess["downloadUrl"] = "/download/{accountId}/{blobId}/{type}/{name}";
    sess["uploadUrl"] = "/upload/{accountId}";
    sess["eventSourceUrl"] = "/events";
    sess["state"] = "0";
    return sess.dump();
}

/// Build a JmapClient with a FakeTransport already primed with a session response.
/// `ft` is shared (not copied) with the client's internal transport_, so responses
/// enqueued by the caller before/after this call and `ft->lastRequest` both observe the
/// exact same underlying FakeTransport the client actually uses.
static JmapClient makeClientWithFakeTransport(const std::shared_ptr<FakeTransport>& ft) {
    // Queue the session GET response.
    ft->enqueueResponse(HttpResponse{
        200,
        { {"Content-Type", "application/json"} },
        makeSessionJson("https://example.com/api")
    });

    JmapClientOptions opts;
    opts.sessionUrl = "https://example.com/.well-known/jmap";
    opts.username = "u";
    opts.password = "p";
    opts.transport = ft;
    JmapClient client(opts);
    client.connect(); // consumes the queued session response
    return client;
}

AJ_TEST(Submission_sendSuccess) {
    auto ft = std::make_shared<FakeTransport>();
    // makeClientWithFakeTransport() enqueues the session GET response AND immediately
    // calls connect(), which consumes it - so the queue (FIFO) must be empty at this
    // point; the EmailSubmission/set response is enqueued AFTER, below, so it's next in
    // line for the method-call POST.
    JmapClient client = makeClientWithFakeTransport(ft);

    // Prepare server response for EmailSubmission/set (create)
    nlohmann::json resp;
    resp["methodResponses"] = nlohmann::json::array({
        nlohmann::json::array({
            "EmailSubmission/set",
            nlohmann::json{
                {"accountId", "a1"},
                {"newState", "1"},
                {"created", {
                    {"c1", {
                        {"id", "s1"},
                        {"sendAt", "2026-08-18T10:00:00Z"},
                        {"undoStatus", "final"}
                    }}
                }}
            },
            "c1"
        })
    });
    resp["sessionState"] = "s1";
    ft->enqueueResponse(HttpResponse{
        200,
        { {"Content-Type", "application/json"} },
        resp.dump()
    });

    EmailSubmission sub;
    sub.identityId = "id1";
    sub.emailId = "e1";

    EmailSubmission result = client.send("a1", sub);

    // Verify merged fields.
    AJ_ASSERT_TRUE(result.id.has_value());
    AJ_ASSERT_EQ(*result.id, "s1");
    AJ_ASSERT_TRUE(result.sendAt.has_value());
    AJ_ASSERT_EQ(*result.sendAt, "2026-08-18T10:00:00Z");
    AJ_ASSERT_TRUE(result.undoStatus.has_value());
    AJ_ASSERT_EQ(*result.undoStatus, "final");
    AJ_ASSERT_EQ(result.identityId, "id1");
    AJ_ASSERT_EQ(result.emailId, "e1");

    // Verify request payload.
    const HttpRequest& req = ft->lastRequest;
    AJ_ASSERT_EQ(req.method, "POST");
    AJ_ASSERT_TRUE(req.url.find("/api") != std::string::npos);
    nlohmann::json sent = nlohmann::json::parse(req.body.value());
    AJ_ASSERT_TRUE(sent.contains("methodCalls"));
    const auto& call = sent["methodCalls"][0];
    AJ_ASSERT_EQ(call[0].get<std::string>(), "EmailSubmission/set");
    AJ_ASSERT_EQ(call[2].get<std::string>(), "c1");
    const auto& args = call[1];
    AJ_ASSERT_EQ(args["accountId"].get<std::string>(), "a1");
    AJ_ASSERT_TRUE(args["create"].contains("c1"));
    AJ_ASSERT_EQ(args["create"]["c1"]["identityId"].get<std::string>(), "id1");
    AJ_ASSERT_EQ(args["create"]["c1"]["emailId"].get<std::string>(), "e1");
}

AJ_TEST(Submission_cancelSend) {
    auto ft = std::make_shared<FakeTransport>();
    JmapClient client = makeClientWithFakeTransport(ft);

    // Server response for cancel (update)
    nlohmann::json resp;
    resp["methodResponses"] = nlohmann::json::array({
        nlohmann::json::array({
            "EmailSubmission/set",
            nlohmann::json{
                {"accountId", "a1"},
                {"newState", "2"}
            },
            "c1"
        })
    });
    resp["sessionState"] = "s1";
    ft->enqueueResponse(HttpResponse{
        200,
        { {"Content-Type", "application/json"} },
        resp.dump()
    });

    client.cancelSend("a1", "s1");

    const HttpRequest& req = ft->lastRequest;
    AJ_ASSERT_EQ(req.method, "POST");
    nlohmann::json sent = nlohmann::json::parse(req.body.value());
    const auto& call = sent["methodCalls"][0];
    AJ_ASSERT_EQ(call[0].get<std::string>(), "EmailSubmission/set");
    const auto& args = call[1];
    AJ_ASSERT_EQ(args["accountId"].get<std::string>(), "a1");
    AJ_ASSERT_TRUE(args["update"].contains("s1"));
    // Regression test: a PatchObject key is a JSON Pointer (RFC 6901) relative to the
    // patched object - a bare top-level property name has no leading slash. A prior
    // version sent "/undoStatus" (pointing at a differently-named property instead),
    // which a real JMAP server rejects/ignores, silently breaking cancel-send.
    AJ_ASSERT_EQ(args["update"]["s1"]["undoStatus"].get<std::string>(), "canceled");
    AJ_ASSERT_TRUE(!args["update"]["s1"].contains("/undoStatus"));
}

AJ_TEST(Submission_listSubmissions) {
    auto ft = std::make_shared<FakeTransport>();
    JmapClient client = makeClientWithFakeTransport(ft);

    // Server response for EmailSubmission/get
    nlohmann::json resp;
    resp["methodResponses"] = nlohmann::json::array({
        nlohmann::json::array({
            "EmailSubmission/get",
            nlohmann::json{
                {"accountId", "a1"},
                {"state", "3"},
                {"list", nlohmann::json::array({
                    nlohmann::json{
                        {"id", "s1"},
                        {"identityId", "id1"},
                        {"emailId", "e1"},
                        {"undoStatus", "final"}
                    },
                    nlohmann::json{
                        {"id", "s2"},
                        {"identityId", "id2"},
                        {"emailId", "e2"},
                        {"undoStatus", "canceled"}
                    }
                })}
            },
            "c1"
        })
    });
    resp["sessionState"] = "s1";
    ft->enqueueResponse(HttpResponse{
        200,
        { {"Content-Type", "application/json"} },
        resp.dump()
    });

    std::vector<EmailSubmission> list = client.listSubmissions("a1");

    AJ_ASSERT_EQ(list.size(), 2u);
    AJ_ASSERT_TRUE(list[0].id.has_value());
    AJ_ASSERT_EQ(*list[0].id, "s1");
    AJ_ASSERT_EQ(list[0].identityId, "id1");
    AJ_ASSERT_EQ(list[0].emailId, "e1");
    AJ_ASSERT_TRUE(list[0].undoStatus.has_value());
    AJ_ASSERT_EQ(*list[0].undoStatus, "final");

    AJ_ASSERT_TRUE(list[1].id.has_value());
    AJ_ASSERT_EQ(*list[1].id, "s2");
    AJ_ASSERT_EQ(list[1].identityId, "id2");
    AJ_ASSERT_EQ(list[1].emailId, "e2");
    AJ_ASSERT_TRUE(list[1].undoStatus.has_value());
    AJ_ASSERT_EQ(*list[1].undoStatus, "canceled");

    // Verify request arguments.
    const HttpRequest& req = ft->lastRequest;
    AJ_ASSERT_EQ(req.method, "POST");
    nlohmann::json sent = nlohmann::json::parse(req.body.value());
    const auto& call = sent["methodCalls"][0];
    AJ_ASSERT_EQ(call[0].get<std::string>(), "EmailSubmission/get");
    const auto& args = call[1];
    AJ_ASSERT_EQ(args["accountId"].get<std::string>(), "a1");
    AJ_ASSERT_TRUE(!args.contains("ids"));
    AJ_ASSERT_TRUE(!args.contains("properties"));
}

AJ_TEST(Submission_sendProtocolError) {
    auto ft = std::make_shared<FakeTransport>();
    JmapClient client = makeClientWithFakeTransport(ft);

    // Server returns an error invocation.
    nlohmann::json resp;
    resp["methodResponses"] = nlohmann::json::array({
        nlohmann::json::array({
            "error",
            nlohmann::json{
                {"type", "invalidArguments"},
                {"description", "Bad request"}
            },
            "c1"
        })
    });
    resp["sessionState"] = "s1";
    ft->enqueueResponse(HttpResponse{
        200,
        { {"Content-Type", "application/json"} },
        resp.dump()
    });

    EmailSubmission sub;
    sub.identityId = "id1";
    sub.emailId = "e1";

    AJ_ASSERT_THROWS(client.send("a1", sub), JmapProtocolError);
}
