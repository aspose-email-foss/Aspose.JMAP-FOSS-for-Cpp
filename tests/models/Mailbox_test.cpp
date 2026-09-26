#include "../test_framework.hpp"
#include "../../include/aspose_jmap/models/Mailbox.hpp"

using namespace aspose_jmap;

/// Helper to create a fully populated Mailbox JSON object.
static nlohmann::json make_full_mailbox_json() {
    return nlohmann::json{
        {"id", "mailbox123"},
        {"name", "Inbox"},
        {"parentId", "parent456"},
        {"role", "inbox"},
        {"sortOrder", 42},
        {"totalEmails", 1000},
        {"unreadEmails", 123},
        {"totalThreads", 800},
        {"unreadThreads", 80},
        {"myRights", {
            {"mayReadItems", true},
            {"mayAddItems", true},
            {"mayRemoveItems", false},
            {"maySetSeen", true},
            {"maySetKeywords", false},
            {"mayCreateChild", false},
            {"mayRename", true},
            {"mayDelete", false},
            {"maySubmit", true}
        }},
        {"isSubscribed", true}
    };
}

/// Helper to create a Mailbox object matching the JSON above.
static Mailbox make_full_mailbox_object() {
    Mailbox m;
    m.id = "mailbox123";
    m.name = "Inbox";
    m.parentId = "parent456";
    m.role = "inbox";
    m.sortOrder = 42;
    m.totalEmails = 1000;
    m.unreadEmails = 123;
    m.totalThreads = 800;
    m.unreadThreads = 80;
    m.myRights = MailboxRights{
        true,  // mayReadItems
        true,  // mayAddItems
        false, // mayRemoveItems
        true,  // maySetSeen
        false, // maySetKeywords
        false, // mayCreateChild
        true,  // mayRename
        false, // mayDelete
        true   // maySubmit
    };
    m.isSubscribed = true;
    return m;
}

AJ_TEST(Mailbox_fromJson_full) {
    const auto json = make_full_mailbox_json();
    Mailbox m = Mailbox::fromJson(json);

    AJ_ASSERT_TRUE(m.id.has_value());
    AJ_ASSERT_EQ(*m.id, "mailbox123");
    AJ_ASSERT_EQ(m.name, "Inbox");
    AJ_ASSERT_TRUE(m.parentId.has_value());
    AJ_ASSERT_EQ(*m.parentId, "parent456");
    AJ_ASSERT_TRUE(m.role.has_value());
    AJ_ASSERT_EQ(*m.role, "inbox");
    AJ_ASSERT_EQ(m.sortOrder, 42u);
    AJ_ASSERT_TRUE(m.totalEmails.has_value());
    AJ_ASSERT_EQ(*m.totalEmails, 1000u);
    AJ_ASSERT_TRUE(m.unreadEmails.has_value());
    AJ_ASSERT_EQ(*m.unreadEmails, 123u);
    AJ_ASSERT_TRUE(m.totalThreads.has_value());
    AJ_ASSERT_EQ(*m.totalThreads, 800u);
    AJ_ASSERT_TRUE(m.unreadThreads.has_value());
    AJ_ASSERT_EQ(*m.unreadThreads, 80u);
    AJ_ASSERT_TRUE(m.myRights.has_value());

    const MailboxRights& r = *m.myRights;
    AJ_ASSERT_TRUE(r.mayReadItems);
    AJ_ASSERT_TRUE(r.mayAddItems);
    AJ_ASSERT_FALSE(r.mayRemoveItems);
    AJ_ASSERT_TRUE(r.maySetSeen);
    AJ_ASSERT_FALSE(r.maySetKeywords);
    AJ_ASSERT_FALSE(r.mayCreateChild);
    AJ_ASSERT_TRUE(r.mayRename);
    AJ_ASSERT_FALSE(r.mayDelete);
    AJ_ASSERT_TRUE(r.maySubmit);

    AJ_ASSERT_TRUE(m.isSubscribed);
}

AJ_TEST(Mailbox_toJson_full) {
    Mailbox m = make_full_mailbox_object();
    nlohmann::json j = m.toJson();
    nlohmann::json expected = make_full_mailbox_json();

    AJ_ASSERT_EQ(j.dump(), expected.dump()); // compare canonical string representation
}

AJ_TEST(Mailbox_roundTrip) {
    Mailbox original = make_full_mailbox_object();
    nlohmann::json j = original.toJson();
    Mailbox roundTrip = Mailbox::fromJson(j);
    AJ_ASSERT_EQ(roundTrip.toJson().dump(), original.toJson().dump());
}

AJ_TEST(Mailbox_fromJson_nullOptionals) {
    nlohmann::json j = {
        {"name", "Archive"},
        {"parentId", nullptr},
        {"role", nullptr},
        {"sortOrder", 0},
        {"isSubscribed", false}
        // omit server‑assigned fields and myRights
    };

    Mailbox m = Mailbox::fromJson(j);

    AJ_ASSERT_FALSE(m.id.has_value());
    AJ_ASSERT_EQ(m.name, "Archive");
    AJ_ASSERT_FALSE(m.parentId.has_value());
    AJ_ASSERT_FALSE(m.role.has_value());
    AJ_ASSERT_EQ(m.sortOrder, 0u);
    AJ_ASSERT_FALSE(m.totalEmails.has_value());
    AJ_ASSERT_FALSE(m.unreadEmails.has_value());
    AJ_ASSERT_FALSE(m.totalThreads.has_value());
    AJ_ASSERT_FALSE(m.unreadThreads.has_value());
    AJ_ASSERT_FALSE(m.myRights.has_value());
    AJ_ASSERT_FALSE(m.isSubscribed);
}

AJ_TEST(Mailbox_fromJson_missingName_throws) {
    nlohmann::json j = {
        {"id", "mailboxX"},
        {"sortOrder", 1}
    };
    AJ_ASSERT_THROWS(Mailbox::fromJson(j), JmapProtocolError);
}
