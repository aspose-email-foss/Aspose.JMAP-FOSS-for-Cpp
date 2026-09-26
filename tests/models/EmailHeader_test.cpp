#include "../test_framework.hpp"
#include "../../include/aspose_jmap/models/EmailHeader.hpp"

using aspose_jmap::EmailHeader;
using aspose_jmap::JmapProtocolError;

AJ_TEST(EmailHeader_roundTripFullData) {
    // Create an EmailHeader instance.
    EmailHeader original;
    original.name = "Subject";
    original.value = "Test email";

    // Serialise to JSON.
    nlohmann::json j = original.toJson();

    // Verify JSON structure.
    AJ_ASSERT_TRUE(j.is_object());
    AJ_ASSERT_EQ(j.at("name").get<std::string>(), "Subject");
    AJ_ASSERT_EQ(j.at("value").get<std::string>(), "Test email");

    // Deserialise back.
    EmailHeader parsed = EmailHeader::fromJson(j);

    // Verify fields match.
    AJ_ASSERT_EQ(parsed.name, original.name);
    AJ_ASSERT_EQ(parsed.value, original.value);
}

AJ_TEST(EmailHeader_fromJson_missingName_throws) {
    // JSON missing the required 'name' field.
    nlohmann::json j = {
        {"value", "Missing name field"}
    };

    AJ_ASSERT_THROWS(EmailHeader::fromJson(j), JmapProtocolError);
}

AJ_TEST(EmailHeader_fromJson_wrongTypeValue_throws) {
    // JSON where 'value' is not a string.
    nlohmann::json j = {
        {"name", "X-Custom"},
        {"value", 12345}
    };

    AJ_ASSERT_THROWS(EmailHeader::fromJson(j), JmapProtocolError);
}
