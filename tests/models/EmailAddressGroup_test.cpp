#include "../test_framework.hpp"
#include "../../include/aspose_jmap/models/EmailAddressGroup.hpp"

using namespace aspose_jmap;

AJ_TEST(EmailAddressGroup_fullRoundTrip) {
    nlohmann::json j = {
        {"name", "Team"},
        {"addresses", nlohmann::json::array({
            { {"email", "a@x"}, {"name", "Alice"} },
            { {"email", "b@x"} }
        })}
    };

    EmailAddressGroup obj = EmailAddressGroup::fromJson(j);

    AJ_ASSERT_TRUE(obj.name.has_value());
    AJ_ASSERT_EQ(*obj.name, "Team");
    AJ_ASSERT_EQ(obj.addresses.size(), 2);
    AJ_ASSERT_EQ(obj.addresses[0].email, "a@x");
    AJ_ASSERT_TRUE(obj.addresses[0].name.has_value());
    AJ_ASSERT_EQ(*obj.addresses[0].name, "Alice");
    AJ_ASSERT_EQ(obj.addresses[1].email, "b@x");
    AJ_ASSERT_FALSE(obj.addresses[1].name.has_value());

    nlohmann::json round = obj.toJson();
    AJ_ASSERT_EQ(round, j);
}

AJ_TEST(EmailAddressGroup_nullNameOmitted) {
    nlohmann::json j = {
        {"name", nullptr},
        {"addresses", nlohmann::json::array({
            { {"email", "c@x"} }
        })}
    };

    EmailAddressGroup obj = EmailAddressGroup::fromJson(j);

    AJ_ASSERT_FALSE(obj.name.has_value());
    AJ_ASSERT_EQ(obj.addresses.size(), 1);
    AJ_ASSERT_EQ(obj.addresses[0].email, "c@x");

    nlohmann::json round = obj.toJson();
    nlohmann::json expected = {
        {"addresses", nlohmann::json::array({
            { {"email", "c@x"} }
        })}
    };
    AJ_ASSERT_EQ(round, expected);
}

AJ_TEST(EmailAddressGroup_missingAddressesThrows) {
    nlohmann::json j = {
        {"name", "Team"}
        // 'addresses' field is intentionally omitted
    };

    AJ_ASSERT_THROWS(EmailAddressGroup::fromJson(j), JmapProtocolError);
}
