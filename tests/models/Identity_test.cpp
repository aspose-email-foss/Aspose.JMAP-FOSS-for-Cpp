#include "../test_framework.hpp"
#include "../../include/aspose_jmap/models/Identity.hpp"

using namespace aspose_jmap;

AJ_TEST(Identity_RoundTripFull) {
    // Build a fully populated Identity
    Identity original;
    original.id = std::string("id123");
    original.name = "Work Identity";
    original.email = "work@example.com";

    EmailAddress reply1;
    reply1.email = "reply1@example.com";
    reply1.name = std::string("Reply One");
    EmailAddress reply2;
    reply2.email = "reply2@example.com"; // name omitted (nullopt)

    original.replyTo = std::vector<EmailAddress>{reply1, reply2};

    EmailAddress bcc1;
    bcc1.email = "bcc1@example.com";
    bcc1.name = std::nullopt;
    original.bcc = std::vector<EmailAddress>{bcc1};

    original.textSignature = "Best regards";
    original.htmlSignature = "<p>Best regards</p>";
    original.mayDelete = true;

    // Serialize to JSON
    nlohmann::json j = original.toJson();

    // Deserialize back
    Identity parsed = Identity::fromJson(j);

    // Verify all fields round‑trip correctly
    AJ_ASSERT_TRUE(parsed.id.has_value());
    AJ_ASSERT_EQ(*parsed.id, "id123");
    AJ_ASSERT_EQ(parsed.name, "Work Identity");
    AJ_ASSERT_EQ(parsed.email, "work@example.com");

    AJ_ASSERT_TRUE(parsed.replyTo.has_value());
    AJ_ASSERT_EQ(parsed.replyTo->size(), 2u);
    AJ_ASSERT_EQ((*parsed.replyTo)[0].email, "reply1@example.com");
    AJ_ASSERT_TRUE((*parsed.replyTo)[0].name.has_value());
    AJ_ASSERT_EQ(*(*parsed.replyTo)[0].name, "Reply One");
    AJ_ASSERT_EQ((*parsed.replyTo)[1].email, "reply2@example.com");
    AJ_ASSERT_TRUE(!(*parsed.replyTo)[1].name.has_value());

    AJ_ASSERT_TRUE(parsed.bcc.has_value());
    AJ_ASSERT_EQ(parsed.bcc->size(), 1u);
    AJ_ASSERT_EQ((*parsed.bcc)[0].email, "bcc1@example.com");
    AJ_ASSERT_TRUE(!(*parsed.bcc)[0].name.has_value());

    AJ_ASSERT_EQ(parsed.textSignature, "Best regards");
    AJ_ASSERT_EQ(parsed.htmlSignature, "<p>Best regards</p>");
    AJ_ASSERT_TRUE(parsed.mayDelete.has_value());
    AJ_ASSERT_TRUE(*parsed.mayDelete);
}

AJ_TEST(Identity_DeserializeMissingOptional) {
    // JSON with only the required field 'email'
    nlohmann::json j = {
        {"email", "minimal@example.com"}
    };

    Identity obj = Identity::fromJson(j);

    // Optional fields should be absent / defaulted
    AJ_ASSERT_TRUE(!obj.id.has_value());
    AJ_ASSERT_EQ(obj.name, "");               // default empty string
    AJ_ASSERT_EQ(obj.email, "minimal@example.com");
    AJ_ASSERT_TRUE(!obj.replyTo.has_value());
    AJ_ASSERT_TRUE(!obj.bcc.has_value());
    AJ_ASSERT_EQ(obj.textSignature, "");      // default empty string
    AJ_ASSERT_EQ(obj.htmlSignature, "");      // default empty string
    AJ_ASSERT_TRUE(!obj.mayDelete.has_value());
}

AJ_TEST(Identity_DeserializeMissingEmailThrows) {
    nlohmann::json j = {
        {"name", "No Email"}
        // 'email' omitted intentionally
    };
    AJ_ASSERT_THROWS(Identity::fromJson(j), JmapProtocolError);
}

AJ_TEST(Identity_DeserializeReplyToNullThrows) {
    nlohmann::json j = {
        {"email", "test@example.com"},
        {"replyTo", nullptr}
    };
    AJ_ASSERT_THROWS(Identity::fromJson(j), JmapProtocolError);
}
