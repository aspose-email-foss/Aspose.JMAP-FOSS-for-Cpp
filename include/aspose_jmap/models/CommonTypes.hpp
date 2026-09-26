#pragma once

#include <string>
#include <vector>
#include <optional>
#include <stdexcept>
#include <nlohmann/json.hpp>

namespace aspose_jmap {

/// Base class for all JMAP-related exceptions.
class JmapError : public std::runtime_error {
public:
    explicit JmapError(const std::string& message) : std::runtime_error(message) {}
};

/// Exception representing a protocol‑level error returned by the JMAP server.
class JmapProtocolError : public JmapError {
public:
    /// The error type identifier, e.g. "unknownMethod", "invalidArguments".
    const std::string type;
    /// Human‑readable description, may be empty.
    const std::string description;

    JmapProtocolError(const std::string& type_, const std::string& description_)
        : JmapError(type_ + (description_.empty() ? "" : (": " + description_))),
          type(type_),
          description(description_) {}

    /// Convenience overload for client-side validation failures that have no real JMAP
    /// server error `type` to report (e.g. a malformed local JSON payload) - `type` is
    /// set to a generic placeholder; the full message is preserved in both `description`
    /// and (via the base class) `what()`.
    explicit JmapProtocolError(const std::string& message)
        : JmapError(message), type("invalidProperties"), description(message) {}
};

/// Exception representing a network‑level failure (e.g. transport errors).
class JmapNetworkError : public JmapError {
public:
    explicit JmapNetworkError(const std::string& message) : JmapError(message) {}
};

/// Primitive type aliases matching the JMAP wire format.
using Id = std::string;          ///< String of 1‑255 base64url characters.
using Int = std::int64_t;        ///< Signed integer, no fractional part.
using UnsignedInt = std::uint64_t; ///< Non‑negative integer, no fractional part.
using Date = std::string;        ///< RFC 3339 date‑time string.
using UTCDate = std::string;     ///< RFC 3339 date‑time string with zero UTC offset.

/// Standard error shape returned per‑id in notCreated / notUpdated / notDestroyed.
struct SetError {
    /// e.g. "invalidProperties", "notFound", "forbidden", "tooLarge".
    std::string type;
    /// Optional human‑readable description.
    std::optional<std::string> description;
    /// Optional list of property names that caused the failure.
    std::optional<std::vector<std::string>> properties;

    /// Construct from a JSON object, validating required fields.
    static SetError fromJson(const nlohmann::json& data) {
        try {
            SetError err;
            err.type = data.at("type").get<std::string>();

            if (data.contains("description")) {
                err.description = data.at("description").get<std::string>();
            }

            if (data.contains("properties")) {
                const auto& arr = data.at("properties");
                if (!arr.is_array()) {
                    throw JmapProtocolError("invalidProperties", "properties must be an array");
                }
                std::vector<std::string> vec;
                for (const auto& el : arr) {
                    vec.emplace_back(el.get<std::string>());
                }
                err.properties = std::move(vec);
            }

            return err;
        } catch (const nlohmann::json::exception& e) {
            throw JmapProtocolError("invalidSetError", e.what());
        }
    }

    /// Serialize to JSON, omitting absent optional members.
    nlohmann::json toJson() const {
        nlohmann::json j;
        j["type"] = type;
        if (description) {
            j["description"] = *description;
        }
        if (properties) {
            j["properties"] = *properties;
        }
        return j;
    }
};

/// Standard top‑level method‑call error returned as an 'error' method response.
struct MethodError {
    /// e.g. "unknownMethod", "invalidArguments", "accountNotFound", "serverFail".
    std::string type;
    /// Optional human‑readable description.
    std::optional<std::string> description;

    static MethodError fromJson(const nlohmann::json& data) {
        try {
            MethodError err;
            err.type = data.at("type").get<std::string>();
            if (data.contains("description")) {
                err.description = data.at("description").get<std::string>();
            }
            return err;
        } catch (const nlohmann::json::exception& e) {
            throw JmapProtocolError("invalidMethodError", e.what());
        }
    }

    nlohmann::json toJson() const {
        nlohmann::json j;
        j["type"] = type;
        if (description) {
            j["description"] = *description;
        }
        return j;
    }
};

/// Back‑reference used inside a request argument to point at a value produced by an earlier method call.
struct ResultReference {
    /// Identifier of the earlier method call.
    std::string resultOf;
    /// Name of the result property to reference.
    std::string name;
    /// JSON Pointer into the referenced result.
    std::string path;

    static ResultReference fromJson(const nlohmann::json& data) {
        try {
            ResultReference ref;
            ref.resultOf = data.at("resultOf").get<std::string>();
            ref.name = data.at("name").get<std::string>();
            ref.path = data.at("path").get<std::string>();
            return ref;
        } catch (const nlohmann::json::exception& e) {
            throw JmapProtocolError("invalidResultReference", e.what());
        }
    }

    nlohmann::json toJson() const {
        nlohmann::json j;
        j["resultOf"] = resultOf;
        j["name"] = name;
        j["path"] = path;
        return j;
    }
};

/*
 * PatchObject (not represented as a C++ type):
 *   A JSON object where each key is a JSON Pointer (RFC 6901) relative to the object being patched,
 *   and each value is either the new value to set at that path or null to remove the path.
 *   Used as the `update` argument shape in every object type's "set" method.
 */

} // namespace aspose_jmap
