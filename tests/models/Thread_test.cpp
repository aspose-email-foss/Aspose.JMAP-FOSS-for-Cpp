#include "../test_framework.hpp"
#include "../../include/aspose_jmap/models/Thread.hpp"

using namespace aspose_jmap;

/// Test full round‑trip (toJson → fromJson) with all fields present.
AJ_TEST(Thread_RoundTripFull) {
    Thread original;
    original.id = std::string("thread123");
    original.emailIds = std::vector<std::string>{"email1", "email2", "email3"};

    nlohmann::json j = original.toJson();

    // Verify JSON contains both members with correct values.
    AJ_ASSERT_TRUE(j.contains("id"));
    AJ_ASSERT_TRUE(j.contains("emailIds"));
    AJ_ASSERT_EQ(j["id"], "thread123");
    AJ_ASSERT_EQ(j["emailIds"].size(), 3);
    AJ_ASSERT_EQ(j["emailIds"][0], "email1");
    AJ_ASSERT_EQ(j["emailIds"][1], "email2");
    AJ_ASSERT_EQ(j["emailIds"][2], "email3");

    Thread parsed = Thread::fromJson(j);

    // Verify the parsed object matches the original.
    AJ_ASSERT_TRUE(parsed.id.has_value());
    AJ_ASSERT_EQ(*parsed.id, "thread123");
    AJ_ASSERT_TRUE(parsed.emailIds.has_value());
    AJ_ASSERT_EQ(parsed.emailIds->size(), 3);
    AJ_ASSERT_EQ((*parsed.emailIds)[0], "email1");
    AJ_ASSERT_EQ((*parsed.emailIds)[1], "email2");
    AJ_ASSERT_EQ((*parsed.emailIds)[2], "email3");
}

/// Test deserialization when optional fields are omitted.
AJ_TEST(Thread_DeserializeMissingOptional) {
    nlohmann::json j = { {"id", "onlyId"} };

    Thread t = Thread::fromJson(j);

    AJ_ASSERT_TRUE(t.id.has_value());
    AJ_ASSERT_EQ(*t.id, "onlyId");
    AJ_ASSERT_TRUE(!t.emailIds.has_value());

    // Serializing back must omit the missing optional field.
    nlohmann::json out = t.toJson();
    AJ_ASSERT_TRUE(out.contains("id"));
    AJ_ASSERT_TRUE(!out.contains("emailIds"));
}

/// Test that an invalid type for `emailIds` triggers a protocol error.
AJ_TEST(Thread_DeserializeInvalidEmailIds) {
    nlohmann::json j = { {"id", "t1"}, {"emailIds", "not-an-array"} };
    AJ_ASSERT_THROWS(Thread::fromJson(j), JmapProtocolError);
}

/// Test serialization of an empty `emailIds` vector (should produce an empty array).
AJ_TEST(Thread_SerializeEmptyEmailIds) {
    Thread t;
    t.id = "tEmpty";
    t.emailIds = std::vector<std::string>{};

    nlohmann::json j = t.toJson();

    AJ_ASSERT_TRUE(j.contains("id"));
    AJ_ASSERT_TRUE(j.contains("emailIds"));
    AJ_ASSERT_EQ(j["emailIds"].size(), 0);
}
