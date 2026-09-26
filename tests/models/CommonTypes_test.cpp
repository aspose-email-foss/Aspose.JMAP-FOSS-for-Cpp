#include "../test_framework.hpp"
#include "../../include/aspose_jmap/models/CommonTypes.hpp"

using namespace aspose_jmap;

/// Test full round‑trip (toJson / fromJson) for SetError with all fields present.
AJ_TEST(CommonTypes_SetError_roundTripFull) {
    SetError original;
    original.type = "invalidProperties";
    original.description = std::make_optional<std::string>("bad data");
    original.properties = std::make_optional<std::vector<std::string>>(std::vector<std::string>{"id", "name"});

    nlohmann::json j = original.toJson();
    // Verify JSON contains all members
    AJ_ASSERT_TRUE(j.contains("type"));
    AJ_ASSERT_TRUE(j.contains("description"));
    AJ_ASSERT_TRUE(j.contains("properties"));
    AJ_ASSERT_EQ(j["type"], "invalidProperties");
    AJ_ASSERT_EQ(j["description"], "bad data");
    AJ_ASSERT_EQ(j["properties"].size(), 2);
    AJ_ASSERT_EQ(j["properties"][0], "id");
    AJ_ASSERT_EQ(j["properties"][1], "name");

    SetError parsed = SetError::fromJson(j);
    AJ_ASSERT_EQ(parsed.type, original.type);
    AJ_ASSERT_TRUE(parsed.description.has_value());
    AJ_ASSERT_EQ(*parsed.description, *original.description);
    AJ_ASSERT_TRUE(parsed.properties.has_value());
    AJ_ASSERT_EQ(*parsed.properties, *original.properties);
}

/// Test SetError serialization when optional fields are omitted.
AJ_TEST(CommonTypes_SetError_missingOptional) {
    SetError original;
    original.type = "notFound";
    // description and properties stay nullopt

    nlohmann::json j = original.toJson();
    AJ_ASSERT_TRUE(j.contains("type"));
    AJ_ASSERT_TRUE(!j.contains("description"));
    AJ_ASSERT_TRUE(!j.contains("properties"));

    SetError parsed = SetError::fromJson(j);
    AJ_ASSERT_EQ(parsed.type, "notFound");
    AJ_ASSERT_TRUE(!parsed.description.has_value());
    AJ_ASSERT_TRUE(!parsed.properties.has_value());
}

/// Test that malformed SetError (properties not an array) throws JmapProtocolError.
AJ_TEST(CommonTypes_SetError_invalidPropertiesNotArray) {
    nlohmann::json bad = {
        {"type", "invalidProperties"},
        {"properties", "shouldBeArray"}
    };
    AJ_ASSERT_THROWS(SetError::fromJson(bad), JmapProtocolError);
}

/// Test full round‑trip for MethodError with optional description.
AJ_TEST(CommonTypes_MethodError_roundTripFull) {
    MethodError original;
    original.type = "unknownMethod";
    original.description = std::make_optional<std::string>("method does not exist");

    nlohmann::json j = original.toJson();
    AJ_ASSERT_TRUE(j.contains("type"));
    AJ_ASSERT_TRUE(j.contains("description"));
    AJ_ASSERT_EQ(j["type"], "unknownMethod");
    AJ_ASSERT_EQ(j["description"], "method does not exist");

    MethodError parsed = MethodError::fromJson(j);
    AJ_ASSERT_EQ(parsed.type, original.type);
    AJ_ASSERT_TRUE(parsed.description.has_value());
    AJ_ASSERT_EQ(*parsed.description, *original.description);
}

/// Test MethodError without optional description.
AJ_TEST(CommonTypes_MethodError_missingOptional) {
    MethodError original;
    original.type = "serverFail";

    nlohmann::json j = original.toJson();
    AJ_ASSERT_TRUE(j.contains("type"));
    AJ_ASSERT_TRUE(!j.contains("description"));

    MethodError parsed = MethodError::fromJson(j);
    AJ_ASSERT_EQ(parsed.type, "serverFail");
    AJ_ASSERT_TRUE(!parsed.description.has_value());
}

/// Test that malformed MethodError (missing required type) throws JmapProtocolError.
AJ_TEST(CommonTypes_MethodError_missingRequired) {
    nlohmann::json bad = {
        {"description", "oops"}
    };
    AJ_ASSERT_THROWS(MethodError::fromJson(bad), JmapProtocolError);
}

/// Test full round‑trip for ResultReference.
AJ_TEST(CommonTypes_ResultReference_roundTrip) {
    ResultReference original;
    original.resultOf = "c1";
    original.name = "mailboxIds";
    original.path = "/0/id";

    nlohmann::json j = original.toJson();
    AJ_ASSERT_TRUE(j.contains("resultOf"));
    AJ_ASSERT_TRUE(j.contains("name"));
    AJ_ASSERT_TRUE(j.contains("path"));
    AJ_ASSERT_EQ(j["resultOf"], "c1");
    AJ_ASSERT_EQ(j["name"], "mailboxIds");
    AJ_ASSERT_EQ(j["path"], "/0/id");

    ResultReference parsed = ResultReference::fromJson(j);
    AJ_ASSERT_EQ(parsed.resultOf, original.resultOf);
    AJ_ASSERT_EQ(parsed.name, original.name);
    AJ_ASSERT_EQ(parsed.path, original.path);
}

/// Test that malformed ResultReference (missing required field) throws JmapProtocolError.
AJ_TEST(CommonTypes_ResultReference_missingRequired) {
    nlohmann::json bad = {
        {"resultOf", "c1"},
        {"name", "mailboxIds"}
        // path missing
    };
    AJ_ASSERT_THROWS(ResultReference::fromJson(bad), JmapProtocolError);
}
