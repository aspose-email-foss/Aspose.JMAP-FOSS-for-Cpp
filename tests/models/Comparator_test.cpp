#include "../test_framework.hpp"
#include "../../include/aspose_jmap/models/Comparator.hpp"

using namespace aspose_jmap;

AJ_TEST(Comparator_roundTripFullData) {
    // Full data with all fields set, including a descending sort and a collation.
    nlohmann::json source = {
        {"property", "subject"},
        {"isAscending", false},
        {"collation", "en-US"}
    };

    Comparator comp = Comparator::fromJson(source);
    AJ_ASSERT_EQ(comp.property, "subject");
    AJ_ASSERT_TRUE(!comp.isAscending);               // false -> descending
    AJ_ASSERT_TRUE(comp.collation.has_value());
    AJ_ASSERT_EQ(*comp.collation, "en-US");

    nlohmann::json roundTrip = comp.toJson();
    // The output must contain all three fields, matching the input.
    AJ_ASSERT_TRUE(roundTrip == source);
}

AJ_TEST(Comparator_roundTripDefaults) {
    // Only the required field; optional fields should take defaults.
    nlohmann::json source = {
        {"property", "size"}
    };

    Comparator comp = Comparator::fromJson(source);
    AJ_ASSERT_EQ(comp.property, "size");
    AJ_ASSERT_TRUE(comp.isAscending);                // default true
    AJ_ASSERT_TRUE(!comp.collation.has_value());

    nlohmann::json roundTrip = comp.toJson();
    // Since isAscending is true (default) and collation is absent, only "property" should be emitted.
    nlohmann::json expected = {
        {"property", "size"}
    };
    AJ_ASSERT_TRUE(roundTrip == expected);
}

AJ_TEST(Comparator_missingRequiredPropertyThrows) {
    // JSON without the required "property" field should trigger a protocol error.
    nlohmann::json bad = {
        {"isAscending", true}
    };

    AJ_ASSERT_THROWS(Comparator::fromJson(bad), JmapProtocolError);
}
