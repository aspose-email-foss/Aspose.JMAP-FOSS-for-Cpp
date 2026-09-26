#pragma once

#include <string>
#include <optional>
#include <nlohmann/json.hpp>
#include "CommonTypes.hpp"

namespace aspose_jmap {

/// Highlighted subject/preview snippet for an Email id matched by a filter's text search.
struct SearchSnippet {
    /// The id of the Email that this snippet refers to.
    std::string emailId;

    /// May contain <mark></mark> tags around matches. Optional.
    std::optional<std::string> subject;

    /// Optional preview snippet.
    std::optional<std::string> preview;

    /// Construct a SearchSnippet from a JSON object.
    /// @throws JmapProtocolError if a required field is missing or has an unexpected type.
    static inline SearchSnippet fromJson(const nlohmann::json& data) {
        SearchSnippet result;

        // emailId – required string
        if (!data.contains("emailId")) {
            throw JmapProtocolError("Missing required field: emailId");
        }
        if (!data.at("emailId").is_string()) {
            throw JmapProtocolError("Field 'emailId' must be a string");
        }
        result.emailId = data.at("emailId").get<std::string>();

        // subject – optional string (may be null)
        if (data.contains("subject") && !data.at("subject").is_null()) {
            if (!data.at("subject").is_string()) {
                throw JmapProtocolError("Field 'subject' must be a string or null");
            }
            result.subject = data.at("subject").get<std::string>();
        } else {
            result.subject = std::nullopt;
        }

        // preview – optional string (may be null)
        if (data.contains("preview") && !data.at("preview").is_null()) {
            if (!data.at("preview").is_string()) {
                throw JmapProtocolError("Field 'preview' must be a string or null");
            }
            result.preview = data.at("preview").get<std::string>();
        } else {
            result.preview = std::nullopt;
        }

        return result;
    }

    /// Serialize this SearchSnippet to a JSON object.
    inline nlohmann::json toJson() const {
        nlohmann::json j = nlohmann::json::object();
        j["emailId"] = emailId;
        if (subject.has_value()) {
            j["subject"] = subject.value();
        }
        if (preview.has_value()) {
            j["preview"] = preview.value();
        }
        return j;
    }
};

} // namespace aspose_jmap
