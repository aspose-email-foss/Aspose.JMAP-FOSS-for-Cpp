#include "../test_framework.hpp"
#include "../../include/aspose_jmap/models/JmapResponseEnvelope.hpp"

using namespace aspose_jmap;

AJ_TEST(JmapResponseEnvelope_RoundTripFull) {
    const std::string raw = R"({
        "methodResponses": [
            ["Mailbox/get", {"list": []}, "c1"]
        ],
        "createdIds": {
            "clientId1": "serverId1",
            "clientId2": "serverId2"
        },
        "sessionState": "abc123"
    })";

    nlohmann::json j = nlohmann::json::parse(raw);
    JmapResponseEnvelope env = JmapResponseEnvelope::fromJson(j);

    // Verify fields
    AJ_ASSERT_EQ(env.sessionState, "abc123");
    AJ_ASSERT_TRUE(env.createdIds.has_value());
    AJ_ASSERT_EQ(env.createdIds->size(), 2u);
    AJ_ASSERT_EQ(env.createdIds->at("clientId1"), "serverId1");
    AJ_ASSERT_EQ(env.createdIds->at("clientId2"), "serverId2");
    AJ_ASSERT_EQ(env.methodResponses.size(), 1u);
    AJ_ASSERT_EQ(env.methodResponses[0].name, "Mailbox/get");
    AJ_ASSERT_EQ(env.methodResponses[0].methodCallId, "c1");

    // Round‑trip serialization
    nlohmann::json j2 = env.toJson();
    AJ_ASSERT_TRUE(j2 == j);
}

AJ_TEST(JmapResponseEnvelope_OptionalCreatedIdsAbsent) {
    const std::string raw = R"({
        "methodResponses": [
            ["Email/get", {"list": []}, "c2"]
        ],
        "sessionState": "state2"
    })";

    nlohmann::json j = nlohmann::json::parse(raw);
    JmapResponseEnvelope env = JmapResponseEnvelope::fromJson(j);

    // createdIds should be absent
    AJ_ASSERT_TRUE(!env.createdIds.has_value());

    // Round‑trip should not emit createdIds
    nlohmann::json j2 = env.toJson();
    AJ_ASSERT_TRUE(!j2.contains("createdIds"));
    AJ_ASSERT_TRUE(j2["methodResponses"] == j["methodResponses"]);
    AJ_ASSERT_EQ(j2["sessionState"], j["sessionState"]);
}

AJ_TEST(JmapResponseEnvelope_OptionalCreatedIdsNull) {
    const std::string raw = R"({
        "methodResponses": [
            ["Email/get", {"list": []}, "c3"]
        ],
        "createdIds": null,
        "sessionState": "state3"
    })";

    nlohmann::json j = nlohmann::json::parse(raw);
    JmapResponseEnvelope env = JmapResponseEnvelope::fromJson(j);

    // Null createdIds should be treated as absent
    AJ_ASSERT_TRUE(!env.createdIds.has_value());

    // Round‑trip should omit createdIds
    nlohmann::json j2 = env.toJson();
    AJ_ASSERT_TRUE(!j2.contains("createdIds"));
    AJ_ASSERT_TRUE(j2["methodResponses"] == j["methodResponses"]);
    AJ_ASSERT_EQ(j2["sessionState"], j["sessionState"]);
}

AJ_TEST(JmapResponseEnvelope_MissingSessionStateThrows) {
    const std::string raw = R"({
        "methodResponses": [
            ["Email/get", {"list": []}, "c4"]
        ]
    })";

    nlohmann::json j = nlohmann::json::parse(raw);
    AJ_ASSERT_THROWS(JmapResponseEnvelope::fromJson(j), JmapProtocolError);
}
