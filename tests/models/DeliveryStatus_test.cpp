#include "../test_framework.hpp"
#include "../../include/aspose_jmap/models/DeliveryStatus.hpp"

AJ_TEST(DeliveryStatus_RoundTripFullData) {
    // Arrange: create a fully populated DeliveryStatus instance.
    aspose_jmap::DeliveryStatus original;
    original.smtpReply = "250 2.0.0 OK";
    original.delivered = "yes";
    original.displayed = "yes";

    // Act: serialize to JSON and deserialize back.
    nlohmann::json j = original.toJson();
    aspose_jmap::DeliveryStatus parsed = aspose_jmap::DeliveryStatus::fromJson(j);

    // Assert: all fields round‑trip unchanged.
    AJ_ASSERT_EQ(parsed.smtpReply, original.smtpReply);
    AJ_ASSERT_EQ(parsed.delivered, original.delivered);
    AJ_ASSERT_EQ(parsed.displayed, original.displayed);

    // Also verify the JSON structure contains the expected keys.
    AJ_ASSERT_TRUE(j.contains("smtpReply"));
    AJ_ASSERT_TRUE(j.contains("delivered"));
    AJ_ASSERT_TRUE(j.contains("displayed"));
    AJ_ASSERT_EQ(j["smtpReply"], original.smtpReply);
    AJ_ASSERT_EQ(j["delivered"], original.delivered);
    AJ_ASSERT_EQ(j["displayed"], original.displayed);
}

AJ_TEST(DeliveryStatus_FromJson_MissingRequiredField_Throws) {
    // Arrange: JSON missing the required "delivered" field.
    nlohmann::json incomplete = {
        {"smtpReply", "250 2.0.0 OK"},
        // "delivered" omitted intentionally
        {"displayed", "yes"}
    };

    // Act & Assert: parsing should throw JmapProtocolError.
    AJ_ASSERT_THROWS(
        aspose_jmap::DeliveryStatus::fromJson(incomplete),
        aspose_jmap::JmapProtocolError
    );
}

AJ_TEST(DeliveryStatus_FromJson_WrongType_Throws) {
    // Arrange: JSON where "smtpReply" is not a string.
    nlohmann::json badType = {
        {"smtpReply", 250},
        {"delivered", "yes"},
        {"displayed", "yes"}
    };

    // Act & Assert: parsing should throw JmapProtocolError.
    AJ_ASSERT_THROWS(
        aspose_jmap::DeliveryStatus::fromJson(badType),
        aspose_jmap::JmapProtocolError
    );
}
