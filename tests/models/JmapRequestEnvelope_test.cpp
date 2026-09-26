#include "../test_framework.hpp"
#include "../../include/aspose_jmap/models/JmapRequestEnvelope.hpp"

using namespace aspose_jmap;

/// Test full round‑trip (serialization then deserialization) with all fields present.
AJ_TEST(JmapRequestEnvelope_RoundTripFull) {
    // Build a request envelope.
    JmapRequestEnvelope env;
    env.using_ = { "urn:ietf:params:jmap:core", "urn:ietf:params:jmap:mail" };

    Invocation inv;
    inv.name = "Mailbox/get";
    inv.arguments = { { "accountId", std::string("user123") } };
    inv.methodCallId = "c1";
    env.methodCalls = { inv };

    env.createdIds = std::map<std::string, std::string>{ { "client1", "server1" } };

    // Serialize to JSON.
    nlohmann::json j = env.toJson();

    // Deserialize back.
    JmapRequestEnvelope parsed = JmapRequestEnvelope::fromJson(j);

    // Verify fields.
    AJ_ASSERT_EQ(parsed.using_, env.using_);
    AJ_ASSERT_EQ(parsed.methodCalls.size(), 1u);
    AJ_ASSERT_EQ(parsed.methodCalls[0].name, inv.name);
    AJ_ASSERT_EQ(parsed.methodCalls[0].arguments.at("accountId").get<std::string>(), "user123");
    AJ_ASSERT_EQ(parsed.methodCalls[0].methodCallId, inv.methodCallId);
    AJ_ASSERT_TRUE(parsed.createdIds.has_value());
    AJ_ASSERT_EQ(parsed.createdIds->at("client1"), "server1");
}

/// Test envelope without the optional `createdIds` field.
AJ_TEST(JmapRequestEnvelope_NoCreatedIds) {
    JmapRequestEnvelope env;
    env.using_ = { "urn:ietf:params:jmap:core" };
    Invocation inv;
    inv.name = "Identity/get";
    inv.arguments = { { "accountId", std::string("acct") } };
    inv.methodCallId = "id2";
    env.methodCalls = { inv };
    // `createdIds` left as std::nullopt.

    nlohmann::json j = env.toJson();
    // The JSON must not contain the key.
    AJ_ASSERT_TRUE(!j.contains("createdIds"));

    JmapRequestEnvelope parsed = JmapRequestEnvelope::fromJson(j);
    AJ_ASSERT_TRUE(!parsed.createdIds.has_value());
    AJ_ASSERT_EQ(parsed.using_, env.using_);
    AJ_ASSERT_EQ(parsed.methodCalls.size(), 1u);
    AJ_ASSERT_EQ(parsed.methodCalls[0].name, inv.name);
}

/// Test that missing required `using` field triggers a protocol error.
AJ_TEST(JmapRequestEnvelope_MissingUsing) {
    nlohmann::json j;
    // Provide required methodCalls (empty array is acceptable).
    j["methodCalls"] = nlohmann::json::array();

    AJ_ASSERT_THROWS(JmapRequestEnvelope::fromJson(j), JmapProtocolError);
}

/// Test that a non‑array `using` field triggers a protocol error.
AJ_TEST(JmapRequestEnvelope_UsingNotArray) {
    nlohmann::json j;
    j["using"] = "urn:ietf:params:jmap:core"; // wrong type
    j["methodCalls"] = nlohmann::json::array();

    AJ_ASSERT_THROWS(JmapRequestEnvelope::fromJson(j), JmapProtocolError);
}
