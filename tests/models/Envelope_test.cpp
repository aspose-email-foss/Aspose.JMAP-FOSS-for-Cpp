#include "../test_framework.hpp"
#include "../../include/aspose_jmap/models/Envelope.hpp"

using namespace aspose_jmap;

/// Test full round‑trip (de)serialization of Envelope with all fields present,
/// including Address parameters with both string and null values.
AJ_TEST(Envelope_roundTripFull) {
    // Build JSON fixture
    nlohmann::json json = {
        {"mailFrom", {
            {"email", "sender@example.com"},
            {"parameters", {
                {"RET", "HDRS"},
                {"SIZE", nullptr}
            }}
        }},
        {"rcptTo", nlohmann::json::array({
            {
                {"email", "rcpt1@example.com"},
                {"parameters", {
                    {"NOTIFY", "SUCCESS"}
                }}
            },
            {
                {"email", "rcpt2@example.com"}  // no parameters field
            }
        })}
    };

    // Deserialize
    Envelope env = Envelope::fromJson(json);

    // Verify fields
    AJ_ASSERT_EQ(env.mailFrom.email, "sender@example.com");
    AJ_ASSERT_TRUE(env.mailFrom.parameters.has_value());
    AJ_ASSERT_EQ(env.mailFrom.parameters->size(), 2);
    AJ_ASSERT_EQ(env.mailFrom.parameters->at("RET").value(), "HDRS");
    AJ_ASSERT_FALSE(env.mailFrom.parameters->at("SIZE").has_value());

    AJ_ASSERT_EQ(env.rcptTo.size(), 2);
    AJ_ASSERT_EQ(env.rcptTo[0].email, "rcpt1@example.com");
    AJ_ASSERT_TRUE(env.rcptTo[0].parameters.has_value());
    AJ_ASSERT_EQ(env.rcptTo[0].parameters->size(), 1);
    AJ_ASSERT_EQ(env.rcptTo[0].parameters->at("NOTIFY").value(), "SUCCESS");

    AJ_ASSERT_EQ(env.rcptTo[1].email, "rcpt2@example.com");
    AJ_ASSERT_FALSE(env.rcptTo[1].parameters.has_value());

    // Serialize back to JSON and compare (order of object members is not significant,
    // so compare the string dumps)
    nlohmann::json roundTrip = env.toJson();
    AJ_ASSERT_EQ(roundTrip.dump(), json.dump());
}

/// Test handling of optional fields: parameters omitted on both mailFrom and rcptTo entries.
AJ_TEST(Envelope_optionalParametersAbsent) {
    nlohmann::json json = {
        {"mailFrom", {
            {"email", "no-params@example.com"}
            // parameters omitted
        }},
        {"rcptTo", nlohmann::json::array({
            {
                {"email", "rcpt@example.com"}
                // parameters omitted
            }
        })}
    };

    Envelope env = Envelope::fromJson(json);

    // mailFrom.parameters should be nullopt
    AJ_ASSERT_FALSE(env.mailFrom.parameters.has_value());

    // rcptTo[0].parameters should be nullopt
    AJ_ASSERT_FALSE(env.rcptTo[0].parameters.has_value());

    // Serialize back – the resulting JSON must not contain "parameters" keys
    nlohmann::json roundTrip = env.toJson();
    AJ_ASSERT_FALSE(roundTrip.at("mailFrom").contains("parameters"));
    AJ_ASSERT_FALSE(roundTrip.at("rcptTo")[0].contains("parameters"));
}

/// Test that missing required field 'email' in Address triggers a protocol error.
AJ_TEST(Address_missingEmail_throws) {
    nlohmann::json bad = {
        {"parameters", {
            {"RET", "HDRS"}
        }}
        // email missing
    };
    AJ_ASSERT_THROWS(Envelope::Address::fromJson(bad), JmapProtocolError);
}

/// Test that missing required field 'mailFrom' in Envelope triggers a protocol error.
AJ_TEST(Envelope_missingMailFrom_throws) {
    nlohmann::json bad = {
        {"rcptTo", nlohmann::json::array({
            { {"email", "rcpt@example.com"} }
        })}
        // mailFrom missing
    };
    AJ_ASSERT_THROWS(Envelope::fromJson(bad), JmapProtocolError);
}
