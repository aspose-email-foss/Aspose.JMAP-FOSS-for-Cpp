#include "../test_framework.hpp"
#include "../../include/aspose_jmap/models/Email.hpp"

using namespace aspose_jmap;

AJ_TEST(Email_fullRoundTrip) {
    nlohmann::json j = {
        {"id", "email-1"},
        {"blobId", "blob-1"},
        {"threadId", "thread-1"},
        {"mailboxIds", { {"mb1", true}, {"mb2", false} }},
        {"keywords", { {"$seen", true}, {"$flagged", false} }},
        {"size", 12345},
        {"receivedAt", "2023-01-01T12:00:00Z"},
        {"messageId", {"<msg1@example.com>", "<msg2@example.com>"}},
        {"inReplyTo", {"<msg0@example.com>"}},
        {"references", {"<msg0@example.com>", "<msg1@example.com>"}},
        {"sender", {
            { {"email", "alice@example.com"}, {"name", "Alice"} }
        }},
        {"from", {
            { {"email", "bob@example.com"} }
        }},
        {"to", {
            { {"email", "carol@example.com"}, {"name", "Carol"} },
            { {"email", "dave@example.com"} }
        }},
        {"cc", nlohmann::json::array()},
        {"bcc", nlohmann::json::array()},
        {"replyTo", {
            { {"email", "reply@example.com"} }
        }},
        {"subject", "Test Subject"},
        {"sentAt", "2023-01-01T11:00:00Z"},
        {"bodyStructure", {
            {"size", 0},
            {"type", "text/plain"},
            {"headers", nlohmann::json::array()}
        }},
        {"bodyValues", {
            {"textBody", {
                {"value", "Hello"},
                {"isEncodingProblem", false},
                {"isTruncated", false}
            }}
        }},
        {"textBody", {
            {
                {"size", 5},
                {"type", "text/plain"},
                {"headers", nlohmann::json::array()}
            }
        }},
        {"htmlBody", nlohmann::json::array()},
        {"attachments", nlohmann::json::array()},
        {"hasAttachment", false},
        {"preview", "Hello world"}
    };

    Email e = Email::fromJson(j);
    nlohmann::json j2 = e.toJson();

    AJ_ASSERT_TRUE(j2 == j);
}

AJ_TEST(Email_optionalFieldsOmitted) {
    nlohmann::json j = {
        {"mailboxIds", { {"inbox", true} }}
        // keywords omitted (should default to empty)
        // all other fields omitted
    };

    Email e = Email::fromJson(j);

    AJ_ASSERT_TRUE(!e.id.has_value());
    AJ_ASSERT_TRUE(e.keywords.empty());

    nlohmann::json j2 = e.toJson();

    AJ_ASSERT_TRUE(j2.contains("mailboxIds"));
    AJ_ASSERT_TRUE(!j2.contains("keywords"));
    AJ_ASSERT_TRUE(!j2.contains("id"));
}

AJ_TEST(Email_missingMailboxIdsThrows) {
    nlohmann::json j = {
        {"id", "email-2"}
        // mailboxIds missing – required field
    };

    AJ_ASSERT_THROWS(Email::fromJson(j), JmapProtocolError);
}
