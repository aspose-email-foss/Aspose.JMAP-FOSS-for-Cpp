#include "../test_framework.hpp"
#include "../../include/aspose_jmap/models/SearchSnippet.hpp"

using namespace aspose_jmap;

AJ_TEST(SearchSnippet_FullRoundTrip) {
    // JSON with all fields present
    nlohmann::json j = {
        {"emailId", "msg-123"},
        {"subject", "Hello <mark>World</mark>"},
        {"preview", "This is a <mark>preview</mark> snippet."}
    };

    // Deserialize
    SearchSnippet ss = SearchSnippet::fromJson(j);
    AJ_ASSERT_EQ(ss.emailId, "msg-123");
    AJ_ASSERT_TRUE(ss.subject.has_value());
    AJ_ASSERT_EQ(ss.subject.value(), "Hello <mark>World</mark>");
    AJ_ASSERT_TRUE(ss.preview.has_value());
    AJ_ASSERT_EQ(ss.preview.value(), "This is a <mark>preview</mark> snippet.");

    // Serialize back
    nlohmann::json j2 = ss.toJson();
    AJ_ASSERT_TRUE(j2.contains("emailId"));
    AJ_ASSERT_TRUE(j2.contains("subject"));
    AJ_ASSERT_TRUE(j2.contains("preview"));
    AJ_ASSERT_EQ(j2["emailId"], "msg-123");
    AJ_ASSERT_EQ(j2["subject"], "Hello <mark>World</mark>");
    AJ_ASSERT_EQ(j2["preview"], "This is a <mark>preview</mark> snippet.");
}

AJ_TEST(SearchSnippet_OptionalAbsent) {
    // JSON with only required field
    nlohmann::json j = {
        {"emailId", "msg-456"}
    };

    SearchSnippet ss = SearchSnippet::fromJson(j);
    AJ_ASSERT_EQ(ss.emailId, "msg-456");
    AJ_ASSERT_TRUE(!ss.subject.has_value());
    AJ_ASSERT_TRUE(!ss.preview.has_value());

    // Serialize should omit optional keys
    nlohmann::json j2 = ss.toJson();
    AJ_ASSERT_TRUE(j2.contains("emailId"));
    AJ_ASSERT_TRUE(!j2.contains("subject"));
    AJ_ASSERT_TRUE(!j2.contains("preview"));
    AJ_ASSERT_EQ(j2["emailId"], "msg-456");
}

AJ_TEST(SearchSnippet_NullOptionalFields) {
    // JSON with optional fields explicitly null
    nlohmann::json j = {
        {"emailId", "msg-789"},
        {"subject", nullptr},
        {"preview", nullptr}
    };

    SearchSnippet ss = SearchSnippet::fromJson(j);
    AJ_ASSERT_EQ(ss.emailId, "msg-789");
    AJ_ASSERT_TRUE(!ss.subject.has_value());
    AJ_ASSERT_TRUE(!ss.preview.has_value());

    // Serialize should still omit them
    nlohmann::json j2 = ss.toJson();
    AJ_ASSERT_TRUE(j2.contains("emailId"));
    AJ_ASSERT_TRUE(!j2.contains("subject"));
    AJ_ASSERT_TRUE(!j2.contains("preview"));
    AJ_ASSERT_EQ(j2["emailId"], "msg-789");
}

AJ_TEST(SearchSnippet_MissingEmailIdThrows) {
    nlohmann::json j = {
        {"subject", "No emailId here"}
    };
    AJ_ASSERT_THROWS(SearchSnippet::fromJson(j), JmapProtocolError);
}

AJ_TEST(SearchSnippet_EmailIdWrongTypeThrows) {
    nlohmann::json j = {
        {"emailId", 12345},
        {"subject", "Invalid type"}
    };
    AJ_ASSERT_THROWS(SearchSnippet::fromJson(j), JmapProtocolError);
}
