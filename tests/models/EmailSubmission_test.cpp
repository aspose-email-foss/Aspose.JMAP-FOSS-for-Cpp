#include "../test_framework.hpp"
#include "../../include/aspose_jmap/models/EmailSubmission.hpp"

using namespace aspose_jmap;

AJ_TEST(EmailSubmission_RoundTripFull) {
    // Build a full EmailSubmission object with all fields populated.
    EmailSubmission original;
    original.id = "subm-123";
    original.threadId = "thread-456";
    original.sendAt = "2023-01-01T12:00:00Z";
    original.undoStatus = "pending";

    // envelope
    Envelope env;
    env.mailFrom.email = "sender@example.com";
    env.mailFrom.parameters = std::nullopt;
    Envelope::Address rcpt;
    rcpt.email = "rcpt@example.com";
    rcpt.parameters = std::nullopt;
    env.rcptTo.push_back(rcpt);
    original.envelope = env;

    // deliveryStatus
    DeliveryStatus ds;
    ds.smtpReply = "250 OK";
    ds.delivered = "yes";
    ds.displayed = "yes";
    original.deliveryStatus = std::map<std::string, DeliveryStatus>{
        {"rcpt@example.com", ds}
    };

    // dsnBlobIds and mdnBlobIds
    original.dsnBlobIds = std::vector<std::string>{"dsn-1", "dsn-2"};
    original.mdnBlobIds = std::vector<std::string>{"mdn-1"};

    // required client fields
    original.identityId = "identity-789";
    original.emailId = "email-321";

    // Serialize to JSON
    nlohmann::json j = original.toJson();

    // Deserialize back
    EmailSubmission parsed = EmailSubmission::fromJson(j);

    // Verify all fields round‑trip correctly
    AJ_ASSERT_TRUE(parsed.id.has_value());
    AJ_ASSERT_EQ(*parsed.id, "subm-123");
    AJ_ASSERT_TRUE(parsed.threadId.has_value());
    AJ_ASSERT_EQ(*parsed.threadId, "thread-456");
    AJ_ASSERT_TRUE(parsed.sendAt.has_value());
    AJ_ASSERT_EQ(*parsed.sendAt, "2023-01-01T12:00:00Z");
    AJ_ASSERT_TRUE(parsed.undoStatus.has_value());
    AJ_ASSERT_EQ(*parsed.undoStatus, "pending");

    AJ_ASSERT_TRUE(parsed.envelope.has_value());
    AJ_ASSERT_EQ(parsed.envelope->mailFrom.email, "sender@example.com");
    AJ_ASSERT_EQ(parsed.envelope->rcptTo.size(), 1u);
    AJ_ASSERT_EQ(parsed.envelope->rcptTo[0].email, "rcpt@example.com");

    AJ_ASSERT_TRUE(parsed.deliveryStatus.has_value());
    AJ_ASSERT_EQ(parsed.deliveryStatus->size(), 1u);
    const auto& dsParsed = parsed.deliveryStatus->at("rcpt@example.com");
    AJ_ASSERT_EQ(dsParsed.smtpReply, "250 OK");
    AJ_ASSERT_EQ(dsParsed.delivered, "yes");
    AJ_ASSERT_EQ(dsParsed.displayed, "yes");

    AJ_ASSERT_TRUE(parsed.dsnBlobIds.has_value());
    AJ_ASSERT_EQ(*parsed.dsnBlobIds, (std::vector<std::string>{"dsn-1", "dsn-2"}));
    AJ_ASSERT_TRUE(parsed.mdnBlobIds.has_value());
    AJ_ASSERT_EQ(*parsed.mdnBlobIds, (std::vector<std::string>{"mdn-1"}));

    AJ_ASSERT_EQ(parsed.identityId, "identity-789");
    AJ_ASSERT_EQ(parsed.emailId, "email-321");
}

AJ_TEST(EmailSubmission_OptionalNullFields) {
    // JSON with only required fields and explicit null envelope
    nlohmann::json j = {
        {"identityId", "identity-001"},
        {"emailId", "email-002"},
        {"envelope", nullptr}
        // all other optional fields omitted
    };

    EmailSubmission parsed = EmailSubmission::fromJson(j);

    // Required fields present
    AJ_ASSERT_EQ(parsed.identityId, "identity-001");
    AJ_ASSERT_EQ(parsed.emailId, "email-002");

    // Optional server‑assigned fields must be nullopt
    AJ_ASSERT_TRUE(!parsed.id.has_value());
    AJ_ASSERT_TRUE(!parsed.threadId.has_value());
    AJ_ASSERT_TRUE(!parsed.sendAt.has_value());
    AJ_ASSERT_TRUE(!parsed.undoStatus.has_value());
    AJ_ASSERT_TRUE(!parsed.deliveryStatus.has_value());
    AJ_ASSERT_TRUE(!parsed.dsnBlobIds.has_value());
    AJ_ASSERT_TRUE(!parsed.mdnBlobIds.has_value());

    // Envelope explicitly null -> nullopt
    AJ_ASSERT_TRUE(!parsed.envelope.has_value());

    // Serializing back should omit all optional fields
    nlohmann::json roundTrip = parsed.toJson();
    AJ_ASSERT_TRUE(roundTrip.contains("identityId"));
    AJ_ASSERT_TRUE(roundTrip.contains("emailId"));
    AJ_ASSERT_TRUE(!roundTrip.contains("id"));
    AJ_ASSERT_TRUE(!roundTrip.contains("threadId"));
    AJ_ASSERT_TRUE(!roundTrip.contains("sendAt"));
    AJ_ASSERT_TRUE(!roundTrip.contains("undoStatus"));
    AJ_ASSERT_TRUE(!roundTrip.contains("deliveryStatus"));
    AJ_ASSERT_TRUE(!roundTrip.contains("dsnBlobIds"));
    AJ_ASSERT_TRUE(!roundTrip.contains("mdnBlobIds"));
    AJ_ASSERT_TRUE(!roundTrip.contains("envelope"));
}

AJ_TEST(EmailSubmission_MissingRequiredThrows) {
    // JSON missing the required 'identityId' field
    nlohmann::json missingIdentity = {
        {"emailId", "email-999"}
    };
    AJ_ASSERT_THROWS(EmailSubmission::fromJson(missingIdentity), JmapProtocolError);

    // JSON missing the required 'emailId' field
    nlohmann::json missingEmail = {
        {"identityId", "identity-999"}
    };
    AJ_ASSERT_THROWS(EmailSubmission::fromJson(missingEmail), JmapProtocolError);
}
