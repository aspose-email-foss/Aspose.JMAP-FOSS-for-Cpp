#pragma once

// Auto-generated composition header (template-rendered, not an LLM task).
// Composes the per-module client mixins into the public JmapClient via virtual
// inheritance, and #includes every model header transitively.

#include <utility>

#include "client_core.hpp"
#include "client_mail.hpp"
#include "client_submission.hpp"
#include "models/CommonTypes.hpp"
#include "models/Transport.hpp"
#include "models/Session.hpp"
#include "models/Invocation.hpp"
#include "models/JmapRequestEnvelope.hpp"
#include "models/JmapResponseEnvelope.hpp"
#include "models/Mailbox.hpp"
#include "models/EmailAddress.hpp"
#include "models/EmailAddressGroup.hpp"
#include "models/EmailBodyPart.hpp"
#include "models/EmailHeader.hpp"
#include "models/Email.hpp"
#include "models/Thread.hpp"
#include "models/Identity.hpp"
#include "models/Comparator.hpp"
#include "models/SearchSnippet.hpp"
#include "models/EmailQueryResponse.hpp"
#include "models/Envelope.hpp"
#include "models/DeliveryStatus.hpp"
#include "models/EmailSubmission.hpp"

namespace aspose_jmap {

class JmapClient : public MailClientMixin, public SubmissionClientMixin {
 public:
  // The virtual base JmapClientCore is initialized directly here (that init is what
  // actually takes effect at runtime, per C++ virtual-inheritance rules); MailClientMixin
  // and SubmissionClientMixin are still listed so each has a valid (if effectively unused)
  // constructor call rather than falling back to their deleted default constructors.
  explicit JmapClient(JmapClientOptions options)
      : JmapClientCore(options), MailClientMixin(options), SubmissionClientMixin(std::move(options)) {}
};

}  // namespace aspose_jmap
