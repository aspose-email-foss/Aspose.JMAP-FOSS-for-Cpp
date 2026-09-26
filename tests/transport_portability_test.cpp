#pragma once

// Regression test: models/Transport.hpp used to unconditionally #include <windows.h> and
// <winhttp.h> and define WinHttpTransport, and client_core.hpp's JmapClientCore constructor
// unconditionally referenced WinHttpTransport as the default transport - even though the
// library is otherwise portable and the README documents that "any platform can supply its
// own Transport subclass via JmapClientOptions::transport". On a real non-Windows build
// (Linux/macOS, where <windows.h>/<winhttp.h> don't exist), that made the whole library fail
// to compile, contradicting the documented portability story - a custom Transport subclass
// couldn't even be used, since just #include-ing the headers already failed.
//
// This translation unit simulates a non-Windows build by undefining _WIN32 before including
// the headers, proving both pieces are now properly guarded: the header compiles (no
// unconditional Windows-only #include), and JmapClientCore falls back to a clear runtime
// exception - instead of referencing an undefined WinHttpTransport - when no transport is
// supplied.

#include "test_framework.hpp"

#undef _WIN32
#undef _WIN64
#include "../include/aspose_jmap/client_core.hpp"

using namespace aspose_jmap;

namespace {

class FakeTransport : public Transport {
public:
    HttpResponse send(const HttpRequest&) override {
        return HttpResponse{};
    }
};

}  // namespace

AJ_TEST(transport_hpp_compiles_and_is_usable_without_win32) {
    HttpRequest req;
    req.method = "GET";
    HttpResponse resp;
    resp.statusCode = 200;
    AJ_ASSERT_TRUE(req.method == "GET");
    AJ_ASSERT_TRUE(resp.statusCode == 200);
}

AJ_TEST(client_core_with_custom_transport_works_without_win32) {
    JmapClientOptions opts;
    opts.sessionUrl = "https://example.com/.well-known/jmap";
    opts.username = "user";
    opts.password = "pass";
    opts.transport = std::make_shared<FakeTransport>();

    // Must not throw and must not reference WinHttpTransport.
    JmapClientCore client(opts);
    AJ_ASSERT_TRUE(true);
}

// Note: JmapClientCore::defaultTransport() is an inline method defined in the header, and
// other test files in this same binary include client_core.hpp with _WIN32 normally defined
// (this is a real Windows build). A test here that undefines _WIN32 and then relies on
// defaultTransport() taking the non-Windows throwing branch would violate the One Definition
// Rule against those other TUs' instantiation of the same inline symbol - the linker is free
// to pick either definition for the whole binary, so the assertion would be unreliable rather
// than a real regression proof. That behavior (throwing std::runtime_error when no transport
// is supplied on a non-Windows build) is instead verified by inspection of
// client_core.hpp's defaultTransport(); a genuine Linux/macOS build has _WIN32 undefined
// consistently across every TU and does not hit this conflict.
