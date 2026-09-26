#include "../test_framework.hpp"
#include "../../include/aspose_jmap/models/EmailBodyPart.hpp"

using namespace aspose_jmap;

/// Normal round‑trip with all fields present.
AJ_TEST(EmailBodyPart_FullRoundTrip) {
    nlohmann::json j = {
        {"partId", "1"},
        {"blobId", "blob123"},
        {"size", 1024},
        {"headers", {
            { {"name", "Content-Type"}, {"value", "text/plain"} },
            { {"name", "X-Custom"}, {"value", "value"} }
        }},
        {"name", "example.txt"},
        {"type", "text/plain"},
        {"charset", "utf-8"},
        {"disposition", "attachment"},
        {"cid", "cid123"},
        {"language", {"en", "fr"}},
        {"location", "http://example.com"},
        {"subParts", {
            {
                {"size", 512},
                {"headers", nlohmann::json::array()},
                {"type", "image/png"}
            }
        }}
    };

    EmailBodyPart obj = EmailBodyPart::fromJson(j);

    // Verify fields
    AJ_ASSERT_TRUE(obj.partId.has_value() && *obj.partId == "1");
    AJ_ASSERT_TRUE(obj.blobId.has_value() && *obj.blobId == "blob123");
    AJ_ASSERT_EQ(obj.size, 1024u);
    AJ_ASSERT_EQ(obj.headers.size(), 2u);
    AJ_ASSERT_TRUE(obj.name.has_value() && *obj.name == "example.txt");
    AJ_ASSERT_EQ(obj.type, "text/plain");
    AJ_ASSERT_TRUE(obj.charset.has_value() && *obj.charset == "utf-8");
    AJ_ASSERT_TRUE(obj.disposition.has_value() && *obj.disposition == "attachment");
    AJ_ASSERT_TRUE(obj.cid.has_value() && *obj.cid == "cid123");
    AJ_ASSERT_TRUE(obj.language.has_value() && obj.language->size() == 2);
    AJ_ASSERT_TRUE(obj.location.has_value() && *obj.location == "http://example.com");
    AJ_ASSERT_TRUE(obj.subParts.has_value() && obj.subParts->size() == 1);
    AJ_ASSERT_EQ((*obj.subParts)[0].size, 512u);
    AJ_ASSERT_EQ((*obj.subParts)[0].type, "image/png");

    // Round‑trip serialization should produce the same JSON (order‑independent)
    AJ_ASSERT_EQ(obj.toJson(), j);
}

/// Edge case: all optional fields omitted.
AJ_TEST(EmailBodyPart_OptionalFieldsAbsent) {
    nlohmann::json j = {
        {"size", 0},
        {"headers", nlohmann::json::array()},
        {"type", "application/octet-stream"}
    };

    EmailBodyPart obj = EmailBodyPart::fromJson(j);

    // Optional fields must be nullopt / empty
    AJ_ASSERT_TRUE(!obj.partId.has_value());
    AJ_ASSERT_TRUE(!obj.blobId.has_value());
    AJ_ASSERT_TRUE(!obj.name.has_value());
    AJ_ASSERT_TRUE(!obj.charset.has_value());
    AJ_ASSERT_TRUE(!obj.disposition.has_value());
    AJ_ASSERT_TRUE(!obj.cid.has_value());
    AJ_ASSERT_TRUE(!obj.language.has_value());
    AJ_ASSERT_TRUE(!obj.location.has_value());
    AJ_ASSERT_TRUE(!obj.subParts.has_value());

    // Required fields
    AJ_ASSERT_EQ(obj.size, 0u);
    AJ_ASSERT_EQ(obj.type, "application/octet-stream");
    AJ_ASSERT_TRUE(obj.headers.empty());

    // Serialized JSON must not contain any of the omitted optional keys
    nlohmann::json serialized = obj.toJson();
    AJ_ASSERT_TRUE(!serialized.contains("partId"));
    AJ_ASSERT_TRUE(!serialized.contains("blobId"));
    AJ_ASSERT_TRUE(!serialized.contains("name"));
    AJ_ASSERT_TRUE(!serialized.contains("charset"));
    AJ_ASSERT_TRUE(!serialized.contains("disposition"));
    AJ_ASSERT_TRUE(!serialized.contains("cid"));
    AJ_ASSERT_TRUE(!serialized.contains("language"));
    AJ_ASSERT_TRUE(!serialized.contains("location"));
    AJ_ASSERT_TRUE(!serialized.contains("subParts"));
    AJ_ASSERT_EQ(serialized["size"], 0);
    AJ_ASSERT_EQ(serialized["type"], "application/octet-stream");
    AJ_ASSERT_TRUE(serialized["headers"].is_array() && serialized["headers"].empty());
}

/// Missing required field 'size' should throw JmapProtocolError.
AJ_TEST(EmailBodyPart_MissingRequiredSize) {
    nlohmann::json j = {
        {"type", "text/plain"},
        {"headers", {}}
    };
    AJ_ASSERT_THROWS(EmailBodyPart::fromJson(j), JmapProtocolError);
}

/// Missing required field 'type' should throw JmapProtocolError.
AJ_TEST(EmailBodyPart_MissingRequiredType) {
    nlohmann::json j = {
        {"size", 123},
        {"headers", {}}
    };
    AJ_ASSERT_THROWS(EmailBodyPart::fromJson(j), JmapProtocolError);
}

/// Missing required field 'headers' should throw JmapProtocolError.
AJ_TEST(EmailBodyPart_MissingRequiredHeaders) {
    nlohmann::json j = {
        {"size", 123},
        {"type", "text/plain"}
    };
    AJ_ASSERT_THROWS(EmailBodyPart::fromJson(j), JmapProtocolError);
}
