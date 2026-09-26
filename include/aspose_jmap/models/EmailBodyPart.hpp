#pragma once

#include <string>
#include <vector>
#include <optional>
#include <cstdint>
#include <nlohmann/json.hpp>

#include "EmailHeader.hpp"
#include "CommonTypes.hpp"

namespace aspose_jmap {

/// One node of the Email's MIME bodyStructure tree.
struct EmailBodyPart {
    std::optional<std::string> partId;                     ///< String|null
    std::optional<std::string> blobId;                     ///< Id|null
    std::uint64_t size;                                    ///< UnsignedInt
    std::vector<EmailHeader> headers;                      ///< EmailHeader[]
    std::optional<std::string> name;                       ///< String|null – filename
    std::string type;                                      ///< String – MIME type
    std::optional<std::string> charset;                    ///< String|null
    std::optional<std::string> disposition;               ///< String|null – e.g. inline, attachment
    std::optional<std::string> cid;                        ///< String|null – Content-Id
    std::optional<std::vector<std::string>> language;     ///< String[]|null
    std::optional<std::string> location;                   ///< String|null
    std::optional<std::vector<EmailBodyPart>> subParts;   ///< EmailBodyPart[]|null

    /// Construct from a JSON object, validating required fields.
    static EmailBodyPart fromJson(const nlohmann::json& data) {
        EmailBodyPart result;

        // Optional scalar fields
        if (data.contains("partId") && !data.at("partId").is_null())
            result.partId = data.at("partId").get<std::string>();
        if (data.contains("blobId") && !data.at("blobId").is_null())
            result.blobId = data.at("blobId").get<std::string>();
        if (data.contains("name") && !data.at("name").is_null())
            result.name = data.at("name").get<std::string>();
        if (data.contains("charset") && !data.at("charset").is_null())
            result.charset = data.at("charset").get<std::string>();
        if (data.contains("disposition") && !data.at("disposition").is_null())
            result.disposition = data.at("disposition").get<std::string>();
        if (data.contains("cid") && !data.at("cid").is_null())
            result.cid = data.at("cid").get<std::string>();
        if (data.contains("location") && !data.at("location").is_null())
            result.location = data.at("location").get<std::string>();

        // Optional array fields
        if (data.contains("language") && !data.at("language").is_null()) {
            const auto& langArr = data.at("language");
            if (!langArr.is_array())
                throw JmapProtocolError("invalidArguments", "Field 'language' must be an array");
            std::vector<std::string> langs;
            for (const auto& el : langArr)
                langs.push_back(el.get<std::string>());
            result.language = std::move(langs);
        }

        if (data.contains("subParts") && !data.at("subParts").is_null()) {
            const auto& spArr = data.at("subParts");
            if (!spArr.is_array())
                throw JmapProtocolError("invalidArguments", "Field 'subParts' must be an array");
            std::vector<EmailBodyPart> parts;
            for (const auto& el : spArr)
                parts.push_back(EmailBodyPart::fromJson(el));
            result.subParts = std::move(parts);
        }

        // Required scalar fields
        try {
            result.size = data.at("size").get<std::uint64_t>();
        } catch (const nlohmann::json::exception&) {
            throw JmapProtocolError("invalidArguments", "Missing required field 'size'");
        }

        try {
            result.type = data.at("type").get<std::string>();
        } catch (const nlohmann::json::exception&) {
            throw JmapProtocolError("invalidArguments", "Missing required field 'type'");
        }

        // Required array field: headers
        try {
            const auto& hdrArr = data.at("headers");
            if (!hdrArr.is_array())
                throw JmapProtocolError("invalidArguments", "Field 'headers' must be an array");
            for (const auto& el : hdrArr)
                result.headers.push_back(EmailHeader::fromJson(el));
        } catch (const nlohmann::json::exception&) {
            throw JmapProtocolError("invalidArguments", "Missing required field 'headers'");
        }

        return result;
    }

    /// Serialize this object to JSON, omitting absent optional fields.
    nlohmann::json toJson() const {
        nlohmann::json j;

        if (partId)      j["partId"] = *partId;
        if (blobId)      j["blobId"] = *blobId;
        j["size"] = size;

        nlohmann::json hdrArr = nlohmann::json::array();
        for (const auto& h : headers)
            hdrArr.push_back(h.toJson());
        j["headers"] = std::move(hdrArr);

        if (name)        j["name"] = *name;
        j["type"] = type;
        if (charset)     j["charset"] = *charset;
        if (disposition)j["disposition"] = *disposition;
        if (cid)         j["cid"] = *cid;
        if (language)    j["language"] = *language;
        if (location)    j["location"] = *location;

        if (subParts) {
            nlohmann::json spArr = nlohmann::json::array();
            for (const auto& p : *subParts)
                spArr.push_back(p.toJson());
            j["subParts"] = std::move(spArr);
        }

        return j;
    }
};

} // namespace aspose_jmap
