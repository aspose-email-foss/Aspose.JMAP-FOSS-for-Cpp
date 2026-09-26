#include "../test_framework.hpp"
#include "../../include/aspose_jmap/models/Session.hpp"

using namespace aspose_jmap;

AJ_TEST(Session_roundTripFullData) {
    // Build a CoreCapability and embed it into the capabilities map.
    CoreCapability coreCap{
        10485760,  // maxSizeUpload
        4,         // maxConcurrentUpload
        1048576,   // maxSizeRequest
        8,         // maxConcurrentRequests
        16,        // maxCallsInRequest
        500,       // maxObjectsInGet
        250,       // maxObjectsInSet
        {"i;unicode-casemap"} // collationAlgorithms
    };
    std::map<std::string, nlohmann::json> capabilities{
        {"urn:ietf:params:jmap:core", coreCap.toJson()}
    };

    // Build a single Account entry.
    Account acc{
        "Test Account",
        true,   // isPersonal
        false,  // isReadOnly
        { {"urn:ietf:params:jmap:mail", nlohmann::json::object()} }
    };
    std::map<std::string, Account> accounts{
        {"acc123", acc}
    };

    // Primary accounts map.
    std::map<std::string, std::string> primaryAccounts{
        {"urn:ietf:params:jmap:mail", "acc123"}
    };

    // Assemble the Session object.
    Session original{
        capabilities,
        accounts,
        primaryAccounts,
        "user@example.com",
        "/jmap/",
        "/download/{accountId}/{blobId}/{type}/{name}",
        "/upload/{accountId}",
        "/eventSource/{accountId}",
        "state123"
    };

    // Serialize to JSON and back.
    nlohmann::json json = original.toJson();
    Session parsed = Session::fromJson(json);

    // Verify round‑trip equality.
    AJ_ASSERT_EQ(parsed.username, original.username);
    AJ_ASSERT_EQ(parsed.apiUrl, original.apiUrl);
    AJ_ASSERT_EQ(parsed.downloadUrl, original.downloadUrl);
    AJ_ASSERT_EQ(parsed.uploadUrl, original.uploadUrl);
    AJ_ASSERT_EQ(parsed.eventSourceUrl, original.eventSourceUrl);
    AJ_ASSERT_EQ(parsed.state, original.state);
    AJ_ASSERT_EQ(parsed.primaryAccounts, original.primaryAccounts);
    AJ_ASSERT_EQ(parsed.accounts.size(), original.accounts.size());
    AJ_ASSERT_TRUE(parsed.accounts.contains("acc123"));
    const Account& parsedAcc = parsed.accounts.at("acc123");
    AJ_ASSERT_EQ(parsedAcc.name, acc.name);
    AJ_ASSERT_EQ(parsedAcc.isPersonal, acc.isPersonal);
    AJ_ASSERT_EQ(parsedAcc.isReadOnly, acc.isReadOnly);
    AJ_ASSERT_EQ(parsedAcc.accountCapabilities, acc.accountCapabilities);
    AJ_ASSERT_TRUE(parsed.capabilities.contains("urn:ietf:params:jmap:core"));
    CoreCapability parsedCore = CoreCapability::fromJson(parsed.capabilities.at("urn:ietf:params:jmap:core"));
    AJ_ASSERT_EQ(parsedCore.maxSizeUpload, coreCap.maxSizeUpload);
    AJ_ASSERT_EQ(parsedCore.maxConcurrentUpload, coreCap.maxConcurrentUpload);
    AJ_ASSERT_EQ(parsedCore.maxSizeRequest, coreCap.maxSizeRequest);
    AJ_ASSERT_EQ(parsedCore.maxConcurrentRequests, coreCap.maxConcurrentRequests);
    AJ_ASSERT_EQ(parsedCore.maxCallsInRequest, coreCap.maxCallsInRequest);
    AJ_ASSERT_EQ(parsedCore.maxObjectsInGet, coreCap.maxObjectsInGet);
    AJ_ASSERT_EQ(parsedCore.maxObjectsInSet, coreCap.maxObjectsInSet);
    AJ_ASSERT_EQ(parsedCore.collationAlgorithms, coreCap.collationAlgorithms);
}

AJ_TEST(Session_missingRequiredFieldThrows) {
    // JSON missing the required "username" field.
    nlohmann::json incomplete = {
        {"capabilities", nlohmann::json::object()},
        {"accounts", nlohmann::json::object()},
        {"primaryAccounts", nlohmann::json::object()},
        // "username" omitted intentionally
        {"apiUrl", "https://example.com/jmap"},
        {"downloadUrl", "https://example.com/download/{accountId}/{blobId}/{type}/{name}"},
        {"uploadUrl", "https://example.com/upload/{accountId}"},
        {"eventSourceUrl", "https://example.com/eventSource/{accountId}"},
        {"state", "stateXYZ"}
    };

    AJ_ASSERT_THROWS(Session::fromJson(incomplete), JmapProtocolError);
}
