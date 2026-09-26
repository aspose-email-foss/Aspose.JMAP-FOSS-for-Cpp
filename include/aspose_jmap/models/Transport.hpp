#pragma once

#include "CommonTypes.hpp"

#include <algorithm>
#include <cctype>
#include <string>
#include <map>
#include <optional>
#include <memory>
#include <stdexcept>
#include <vector>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winhttp.h>
#pragma comment(lib, "winhttp.lib")
#endif

namespace aspose_jmap {

/// A low-level HTTP request used by the JMAP client core.
struct HttpRequest {
    std::string method;
    std::string url;
    std::map<std::string, std::string> headers;
    std::optional<std::string> body;
};

/// A low-level HTTP response returned by a Transport implementation.
struct HttpResponse {
    int statusCode = 0;
    std::map<std::string, std::string> headers;
    std::string body;
};

/// Abstract transport interface for sending HTTP requests.
class Transport {
public:
    virtual ~Transport() = default;

    /// Sends an HTTP request and returns the response.
    /// @param request The HTTP request to send.
    /// @return The HTTP response.
    virtual HttpResponse send(const HttpRequest& request) = 0;
};

#ifdef _WIN32
/// Default WinHTTP based transport implementation (Windows only). Guarded by `_WIN32` so
/// that this header - and therefore the rest of the library, which is otherwise portable -
/// remains includable on platforms where <windows.h>/<winhttp.h> don't exist; supply a
/// custom Transport subclass via JmapClientOptions::transport there instead.
class WinHttpTransport : public Transport {
public:
    WinHttpTransport() = default;

    /// Sends the request, following redirects manually (WinHTTP's own auto-redirect is
    /// disabled per hop). Every original header - including `Authorization` - is re-sent on
    /// a SAME-ORIGIN redirect; on a CROSS-ORIGIN redirect the credential headers
    /// (`Authorization` / `Cookie` / `Proxy-Authorization`) are dropped before the next
    /// hop, so a hostile redirect cannot leak them to another host. Same-origin redirects
    /// (e.g. Stalwart's `/.well-known/jmap` -> session endpoint) keep auth.
    inline HttpResponse send(const HttpRequest& request) override {
        const std::string startOrigin = originOf(request.url);
        std::string currentUrl = request.url;
        bool stripCredentials = false;
        constexpr int kMaxRedirects = 5;

        for (int hop = 0;; ++hop) {
            HttpResponse resp = sendOnce(currentUrl, request, stripCredentials);

            const bool isRedirect = resp.statusCode == 301 || resp.statusCode == 302
                || resp.statusCode == 303 || resp.statusCode == 307 || resp.statusCode == 308;
            if (!isRedirect || hop >= kMaxRedirects) {
                return resp;
            }
            std::string location;
            for (const auto& [k, v] : resp.headers) {
                if (iequals(k, "Location")) { location = v; break; }
            }
            if (location.empty()) {
                return resp;
            }
            currentUrl = resolveLocation(currentUrl, location);
            if (originOf(currentUrl) != startOrigin) {
                stripCredentials = true;
            }
        }
    }

private:
    static bool iequals(const std::string& a, const std::string& b) {
        if (a.size() != b.size()) return false;
        for (size_t i = 0; i < a.size(); ++i) {
            if (std::tolower((unsigned char)a[i]) != std::tolower((unsigned char)b[i])) return false;
        }
        return true;
    }

    static bool isCredentialHeader(const std::string& name) {
        return iequals(name, "Authorization") || iequals(name, "Cookie")
            || iequals(name, "Proxy-Authorization");
    }

    /// scheme://host:port (lowercased scheme+host, explicit default port) for same-origin
    /// comparison. Falls back to the raw string if the URL can't be parsed.
    static std::string originOf(const std::string& url) {
        URL_COMPONENTS uc{};
        uc.dwStructSize = sizeof(uc);
        uc.dwSchemeLength = (DWORD)-1;
        uc.dwHostNameLength = (DWORD)-1;
        std::wstring w(url.begin(), url.end());
        if (!WinHttpCrackUrl(w.c_str(), 0, 0, &uc)) return url;
        std::wstring scheme(uc.lpszScheme, uc.dwSchemeLength);
        std::wstring host(uc.lpszHostName, uc.dwHostNameLength);
        std::string s(scheme.begin(), scheme.end());
        std::string h(host.begin(), host.end());
        std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return (char)std::tolower(c); });
        std::transform(h.begin(), h.end(), h.begin(), [](unsigned char c){ return (char)std::tolower(c); });
        return s + "://" + h + ":" + std::to_string((int)uc.nPort);
    }

    /// Resolves a `Location` value against the current request URL. Handles an absolute
    /// URL, a scheme-relative `//host/...`, and a root-relative `/path`.
    static std::string resolveLocation(const std::string& base, const std::string& loc) {
        auto startsWith = [](const std::string& s, const char* p) {
            return s.rfind(p, 0) == 0;
        };
        if (startsWith(loc, "http://") || startsWith(loc, "https://")) return loc;
        // origin without the port suffix originOf adds: rebuild scheme://host
        URL_COMPONENTS uc{};
        uc.dwStructSize = sizeof(uc);
        uc.dwSchemeLength = (DWORD)-1;
        uc.dwHostNameLength = (DWORD)-1;
        std::wstring w(base.begin(), base.end());
        if (!WinHttpCrackUrl(w.c_str(), 0, 0, &uc)) return loc;
        std::wstring scheme(uc.lpszScheme, uc.dwSchemeLength);
        std::wstring host(uc.lpszHostName, uc.dwHostNameLength);
        std::string s(scheme.begin(), scheme.end());
        std::string h(host.begin(), host.end());
        std::string portSuffix;
        if (!((iequals(s, "https") && uc.nPort == 443) || (iequals(s, "http") && uc.nPort == 80))) {
            portSuffix = ":" + std::to_string((int)uc.nPort);
        }
        if (startsWith(loc, "//")) return s + ":" + loc;
        std::string origin = s + "://" + h + portSuffix;
        if (startsWith(loc, "/")) return origin + loc;
        return origin + "/" + loc;
    }

    inline HttpResponse sendOnce(const std::string& targetUrl, const HttpRequest& request,
                                 bool stripCredentials) {
        // --------------------------------------------------------------------
        // Parse the URL using WinHttpCrackUrl.
        // --------------------------------------------------------------------
        URL_COMPONENTS urlComp{};
        urlComp.dwStructSize = sizeof(urlComp);
        urlComp.dwSchemeLength   = (DWORD)-1;
        urlComp.dwHostNameLength = (DWORD)-1;
        urlComp.dwUrlPathLength  = (DWORD)-1;
        urlComp.dwExtraInfoLength = (DWORD)-1;

        std::wstring wUrl(targetUrl.begin(), targetUrl.end());
        if (!WinHttpCrackUrl(wUrl.c_str(), 0, 0, &urlComp)) {
            throw JmapNetworkError("WinHttpCrackUrl failed: " + std::to_string(GetLastError()));
        }

        std::wstring wScheme(urlComp.lpszScheme, urlComp.dwSchemeLength);
        std::wstring wHost(urlComp.lpszHostName, urlComp.dwHostNameLength);
        std::wstring wPath(urlComp.lpszUrlPath, urlComp.dwUrlPathLength);
        std::wstring wExtra(urlComp.lpszExtraInfo, urlComp.dwExtraInfoLength);
        std::wstring wFullPath = wPath + wExtra;

        // Helper to convert wide strings (UTF‑16) to UTF‑8 std::string.
        auto toUtf8 = [](const std::wstring& ws) -> std::string {
            if (ws.empty()) return {};
            int size = WideCharToMultiByte(CP_UTF8, 0, ws.c_str(), -1, nullptr, 0, nullptr, nullptr);
            std::string s(size - 1, 0);
            WideCharToMultiByte(CP_UTF8, 0, ws.c_str(), -1, &s[0], size, nullptr, nullptr);
            return s;
        };

        std::string host = toUtf8(wHost);
        std::string path = toUtf8(wFullPath);
        bool isSecure = (_wcsicmp(wScheme.c_str(), L"https") == 0);

        // --------------------------------------------------------------------
        // Open a WinHTTP session.
        // --------------------------------------------------------------------
        HINTERNET hSession = WinHttpOpen(L"aspose-jmap/1.0",
                                         WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                                         WINHTTP_NO_PROXY_NAME,
                                         WINHTTP_NO_PROXY_BYPASS,
                                         0);
        if (!hSession) {
            throw JmapNetworkError("WinHttpOpen failed: " + std::to_string(GetLastError()));
        }
        struct SessionHandle {
            HINTERNET handle;
            ~SessionHandle() { if (handle) WinHttpCloseHandle(handle); }
        } session{hSession};

        // --------------------------------------------------------------------
        // Connect to the host.
        // --------------------------------------------------------------------
        HINTERNET hConnect = WinHttpConnect(session.handle, wHost.c_str(),
                                            urlComp.nPort, 0);
        if (!hConnect) {
            throw JmapNetworkError("WinHttpConnect failed: " + std::to_string(GetLastError()));
        }
        struct ConnectHandle {
            HINTERNET handle;
            ~ConnectHandle() { if (handle) WinHttpCloseHandle(handle); }
        } connect{hConnect};

        // --------------------------------------------------------------------
        // Create the request.
        // --------------------------------------------------------------------
        DWORD flags = isSecure ? WINHTTP_FLAG_SECURE : 0;
        std::wstring wMethod(request.method.begin(), request.method.end());

        HINTERNET hRequest = WinHttpOpenRequest(connect.handle,
                                                wMethod.c_str(),
                                                wFullPath.c_str(),
                                                nullptr,
                                                WINHTTP_NO_REFERER,
                                                WINHTTP_DEFAULT_ACCEPT_TYPES,
                                                flags);
        if (!hRequest) {
            throw JmapNetworkError("WinHttpOpenRequest failed: " + std::to_string(GetLastError()));
        }
        struct RequestHandle {
            HINTERNET handle;
            ~RequestHandle() { if (handle) WinHttpCloseHandle(handle); }
        } req{hRequest};

        // Disable WinHTTP's own redirect following - send() drives redirects itself so it
        // can drop credentials on a cross-origin hop. A 3xx then surfaces to us as a normal
        // response with a Location header.
        DWORD redirectPolicy = WINHTTP_OPTION_REDIRECT_POLICY_NEVER;
        WinHttpSetOption(req.handle, WINHTTP_OPTION_REDIRECT_POLICY,
                         &redirectPolicy, sizeof(redirectPolicy));

        // --------------------------------------------------------------------
        // Add request headers. On a cross-origin redirect hop, the credential
        // headers (Authorization / Cookie / Proxy-Authorization) are omitted.
        // --------------------------------------------------------------------
        {
            std::string headerStr;
            for (const auto& [key, value] : request.headers) {
                if (stripCredentials && isCredentialHeader(key)) continue;
                headerStr += key + ": " + value + "\r\n";
            }
            if (!headerStr.empty()) {
                std::wstring wHeaders(headerStr.begin(), headerStr.end());
                if (!WinHttpAddRequestHeaders(req.handle,
                                              wHeaders.c_str(),
                                              (DWORD)-1,
                                              WINHTTP_ADDREQ_FLAG_ADD | WINHTTP_ADDREQ_FLAG_REPLACE)) {
                    throw JmapNetworkError("WinHttpAddRequestHeaders failed: " + std::to_string(GetLastError()));
                }
            }
        }

        // --------------------------------------------------------------------
        // Send the request (with optional body).
        // --------------------------------------------------------------------
        LPCVOID bodyData = nullptr;
        DWORD   bodyLength = 0;
        std::vector<char> bodyBytes;
        if (request.body) {
            const std::string& bodyStr = *request.body;
            bodyBytes.assign(bodyStr.begin(), bodyStr.end());
            bodyData   = bodyBytes.data();
            bodyLength = static_cast<DWORD>(bodyBytes.size());
        }

        if (!WinHttpSendRequest(req.handle,
                                WINHTTP_NO_ADDITIONAL_HEADERS,
                                0,
                                const_cast<LPVOID>(bodyData),
                                bodyLength,
                                bodyLength,
                                0)) {
            throw JmapNetworkError("WinHttpSendRequest failed: " + std::to_string(GetLastError()));
        }

        if (!WinHttpReceiveResponse(req.handle, nullptr)) {
            throw JmapNetworkError("WinHttpReceiveResponse failed: " + std::to_string(GetLastError()));
        }

        // --------------------------------------------------------------------
        // Retrieve the status code.
        // --------------------------------------------------------------------
        DWORD statusCode = 0;
        DWORD statusCodeSize = sizeof(statusCode);
        if (!WinHttpQueryHeaders(req.handle,
                                 WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                                 WINHTTP_HEADER_NAME_BY_INDEX,
                                 &statusCode,
                                 &statusCodeSize,
                                 WINHTTP_NO_HEADER_INDEX)) {
            throw JmapNetworkError("WinHttpQueryHeaders (status) failed: " + std::to_string(GetLastError()));
        }

        // --------------------------------------------------------------------
        // Retrieve raw response headers.
        // --------------------------------------------------------------------
        std::map<std::string, std::string> responseHeaders;
        DWORD headerBufSize = 0;
        // First call to obtain required buffer size.
        WinHttpQueryHeaders(req.handle,
                            WINHTTP_QUERY_RAW_HEADERS_CRLF,
                            WINHTTP_HEADER_NAME_BY_INDEX,
                            nullptr,
                            &headerBufSize,
                            WINHTTP_NO_HEADER_INDEX);
        if (GetLastError() == ERROR_INSUFFICIENT_BUFFER && headerBufSize > 0) {
            std::vector<wchar_t> headerBuf(headerBufSize);
            if (WinHttpQueryHeaders(req.handle,
                                    WINHTTP_QUERY_RAW_HEADERS_CRLF,
                                    WINHTTP_HEADER_NAME_BY_INDEX,
                                    headerBuf.data(),
                                    &headerBufSize,
                                    WINHTTP_NO_HEADER_INDEX)) {
                std::wstring wAllHeaders(headerBuf.data(), headerBufSize);
                std::string allHeaders = toUtf8(wAllHeaders);
                size_t start = 0;
                while (start < allHeaders.size()) {
                    size_t end = allHeaders.find("\r\n", start);
                    if (end == std::string::npos) break;
                    std::string line = allHeaders.substr(start, end - start);
                    start = end + 2;
                    auto colonPos = line.find(':');
                    if (colonPos != std::string::npos) {
                        std::string name  = line.substr(0, colonPos);
                        std::string value = line.substr(colonPos + 1);
                        while (!value.empty() && (value.front() == ' ' || value.front() == '\t')) {
                            value.erase(value.begin());
                        }
                        responseHeaders.emplace(std::move(name), std::move(value));
                    }
                }
            } else {
                throw JmapNetworkError("WinHttpQueryHeaders (raw) failed: " + std::to_string(GetLastError()));
            }
        }

        // --------------------------------------------------------------------
        // Read the response body.
        // --------------------------------------------------------------------
        std::string responseBody;
        DWORD bytesAvailable = 0;
        while (WinHttpQueryDataAvailable(req.handle, &bytesAvailable) && bytesAvailable > 0) {
            std::vector<char> buffer(bytesAvailable);
            DWORD bytesRead = 0;
            if (!WinHttpReadData(req.handle, buffer.data(), bytesAvailable, &bytesRead)) {
                throw JmapNetworkError("WinHttpReadData failed: " + std::to_string(GetLastError()));
            }
            responseBody.append(buffer.data(), bytesRead);
        }

        // --------------------------------------------------------------------
        // Populate and return the HttpResponse.
        // --------------------------------------------------------------------
        HttpResponse resp;
        resp.statusCode = static_cast<int>(statusCode);
        resp.headers    = std::move(responseHeaders);
        resp.body       = std::move(responseBody);
        return resp;
    }
};
#endif // _WIN32

} // namespace aspose_jmap
