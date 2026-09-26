#include "../test_framework.hpp"
#include "../../include/aspose_jmap/models/EmailAddress.hpp"

using namespace aspose_jmap;

/// Test full round‑trip (both fields present).
AJ_TEST(EmailAddress_fullRoundTrip) {
    nlohmann::json src = {
        {"name", "John Doe"},
        {"email", "john@example.com"}
    };

    // Deserialize
    EmailAddress addr = EmailAddress::fromJson(src);
    AJ_ASSERT_TRUE(addr.name.has_value());
    AJ_ASSERT_EQ(*addr.name, "John Doe");
    AJ_ASSERT_EQ(addr.email, "john@example.com");

    // Serialize back
    nlohmann::json out = addr.toJson();
    AJ_ASSERT_EQ(out, src);
}

/// Test deserialization when the optional `name` field is omitted.
AJ_TEST(EmailAddress_missingOptionalName) {
    nlohmann::json src = {
        {"email", "jane@example.com"}
    };

    EmailAddress addr = EmailAddress::fromJson(src);
    AJ_ASSERT_TRUE(!addr.name.has_value());
    AJ_ASSERT_EQ(addr.email, "jane@example.com");

    // Serialized form must not contain the `name` key.
    nlohmann::json expected = {
        {"email", "jane@example.com"}
    };
    AJ_ASSERT_EQ(addr.toJson(), expected);
}

/// Test deserialization when the optional `name` field is explicitly null.
AJ_TEST(EmailAddress_nullOptionalName) {
    nlohmann::json src = {
        {"name", nullptr},
        {"email", "jane@example.com"}
    };

    EmailAddress addr = EmailAddress::fromJson(src);
    AJ_ASSERT_TRUE(!addr.name.has_value());
    AJ_ASSERT_EQ(addr.email, "jane@example.com");

    // Serialized form must omit the `name` key.
    nlohmann::json expected = {
        {"email", "jane@example.com"}
    };
    AJ_ASSERT_EQ(addr.toJson(), expected);
}

/// Test that missing required `email` field throws a protocol error.
AJ_TEST(EmailAddress_missingRequiredEmail) {
    nlohmann::json src = {
        {"name", "Bob"}
    };
    AJ_ASSERT_THROWS(EmailAddress::fromJson(src), JmapProtocolError);
}

/// Test that an incorrectly typed `email` field throws a protocol error.
AJ_TEST(EmailAddress_invalidEmailType) {
    nlohmann::json src = {
        {"email", 12345}
    };
    AJ_ASSERT_THROWS(EmailAddress::fromJson(src), JmapProtocolError);
}
