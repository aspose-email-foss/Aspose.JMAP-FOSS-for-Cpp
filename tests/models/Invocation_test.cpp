#include "../test_framework.hpp"
#include "../../include/aspose_jmap/models/Invocation.hpp"

using namespace aspose_jmap;

AJ_TEST(Invocation_roundTripFullData)
{
    // Build a JSON array representing a valid Invocation
    nlohmann::json jsonArray = nlohmann::json::array({
        "Mailbox/get",
        nlohmann::json::object({ {"accountId", "a1"} }),
        "c1"
    });

    // Deserialize
    Invocation inv = Invocation::fromJson(jsonArray);

    // Verify fields
    AJ_ASSERT_EQ(inv.name, "Mailbox/get");
    AJ_ASSERT_EQ(inv.methodCallId, "c1");
    AJ_ASSERT_TRUE(inv.arguments.contains("accountId"));
    AJ_ASSERT_EQ(inv.arguments.at("accountId").get<std::string>(), "a1");

    // Serialize back to JSON and compare with original
    nlohmann::json roundTrip = inv.toJson();
    AJ_ASSERT_TRUE(roundTrip.is_array());
    AJ_ASSERT_EQ(roundTrip.size(), 3);
    AJ_ASSERT_EQ(roundTrip.at(0).get<std::string>(), "Mailbox/get");
    AJ_ASSERT_EQ(roundTrip.at(2).get<std::string>(), "c1");
    AJ_ASSERT_TRUE(roundTrip.at(1).is_object());
    AJ_ASSERT_EQ(roundTrip.at(1).at("accountId").get<std::string>(), "a1");
}

AJ_TEST(Invocation_invalidNotArray)
{
    // JSON object instead of array should trigger protocol error
    nlohmann::json notArray = nlohmann::json::object({ {"name", "Mailbox/get"} });
    AJ_ASSERT_THROWS(Invocation::fromJson(notArray), JmapProtocolError);
}

AJ_TEST(Invocation_invalidWrongSize)
{
    // Array with wrong number of elements
    nlohmann::json tooShort = nlohmann::json::array({ "Mailbox/get", nlohmann::json::object() });
    AJ_ASSERT_THROWS(Invocation::fromJson(tooShort), JmapProtocolError);

    nlohmann::json tooLong = nlohmann::json::array({ "Mailbox/get", nlohmann::json::object(), "c1", "extra" });
    AJ_ASSERT_THROWS(Invocation::fromJson(tooLong), JmapProtocolError);
}

AJ_TEST(Invocation_invalidElementTypes)
{
    // First element not a string
    nlohmann::json badName = nlohmann::json::array({ 123, nlohmann::json::object(), "c1" });
    AJ_ASSERT_THROWS(Invocation::fromJson(badName), JmapProtocolError);

    // Second element not an object
    nlohmann::json badArgs = nlohmann::json::array({ "Mailbox/get", "not-an-object", "c1" });
    AJ_ASSERT_THROWS(Invocation::fromJson(badArgs), JmapProtocolError);

    // Third element not a string
    nlohmann::json badId = nlohmann::json::array({ "Mailbox/get", nlohmann::json::object(), 456 });
    AJ_ASSERT_THROWS(Invocation::fromJson(badId), JmapProtocolError);
}
