
#include "common/http/http1/llhttp_parser.h"

#include <llhttp.h>

#include "common/common/assert.h"
#include "common/http/http1/parser.h"

namespace Envoy {
namespace Http {
namespace Http1 {

class LlhttpHttpParserImpl::Impl {
public:
  Impl(llhttp_type type, void* data) {
    settings_ = {
        [](llhttp_t* parser) -> int {
          return static_cast<ParserCallbacks*>(parser->data)->onMessageBegin();
        },
        [](llhttp_t* parser, const char* at, size_t length) -> int {
          return static_cast<ParserCallbacks*>(parser->data)->onUrl(at, length);
        },
        // TODO(dereka) onStatus
        nullptr,
        [](llhttp_t* parser, const char* at, size_t length) -> int {
          return static_cast<ParserCallbacks*>(parser->data)->onHeaderField(at, length);
        },
        [](llhttp_t* parser, const char* at, size_t length) -> int {
          return static_cast<ParserCallbacks*>(parser->data)->onHeaderValue(at, length);
        },
        [](llhttp_t* parser) -> int {
          return static_cast<ParserCallbacks*>(parser->data)->onHeadersComplete();
        },
        [](llhttp_t* parser, const char* at, size_t length) -> int {
          return static_cast<ParserCallbacks*>(parser->data)->bufferBody(at, length);
        },
        [](llhttp_t* parser) -> int {
          return static_cast<ParserCallbacks*>(parser->data)->onMessageComplete();
        },
        [](llhttp_t* parser) -> int {
          // A 0-byte chunk header is used to signal the end of the chunked body.
          // When this function is called, http-parser holds the size of the chunk in
          // parser->content_length. See
          // https://github.com/nodejs/http-parser/blob/v2.9.3/http_parser.h#L336
          const bool is_final_chunk = (parser->content_length == 0);
          return static_cast<ParserCallbacks*>(parser->data)->onChunkHeader(is_final_chunk);
        },
        nullptr,  // TODO(dereka) onChunkComplete
        nullptr,  // on_url_complete
        nullptr,  // on_status_complete
        nullptr,  // on_header_field_complete
        nullptr   // on_header_value_complete
    };
    llhttp_init(&parser_, type, &settings_);
    parser_.data = data;
  }

  size_t execute(const char* slice, int len) {
    llhttp_errno_t err;
    if (slice == nullptr || len == 0) {
      err = llhttp_finish(&parser_);
    } else {
      err = llhttp_execute(&parser_, slice, len);
    }

    size_t nread = len;
    if (err != HPE_OK) {
      nread = llhttp_get_error_pos(&parser_) - slice;
      if (err == HPE_PAUSED_UPGRADE) {
        err = HPE_OK;
        llhttp_resume_after_upgrade(&parser_);
      }
    }

    return nread;
  }

  void resume() { llhttp_resume(&parser_); }

  int pause() {
    llhttp_pause(&parser_);
    return HPE_PAUSED;
  }

  int getErrno() { return llhttp_get_errno(&parser_); }

  int statusCode() const { return parser_.status_code; }

  int httpMajor() const { return parser_.http_major; }

  int httpMinor() const { return parser_.http_minor; }

  uint64_t contentLength() const { return parser_.content_length; }

  int flags() const { return parser_.flags; }

  uint16_t method() const { return parser_.method; }

  const char* methodName() const {
    return llhttp_method_name(static_cast<llhttp_method>(parser_.method));
  }

  int usesTransferEncoding() const { return 0; /* parser_.uses_transfer_encoding; */}

private:
  llhttp_t parser_;
  llhttp_settings_s settings_;
};

LlhttpHttpParserImpl::LlhttpHttpParserImpl(MessageType type, ParserCallbacks* data) {
  llhttp_type_t parser_type;
  switch (type) {
  case MessageType::Request:
    parser_type = HTTP_REQUEST;
    break;
  case MessageType::Response:
    parser_type = HTTP_RESPONSE;
    break;
  default:
    NOT_IMPLEMENTED_GCOVR_EXCL_LINE;
  }

  impl_ = std::make_unique<Impl>(parser_type, data);
}

// Because we have a pointer-to-impl using std::unique_ptr, we must place the destructor in the
// same compilation unit so that the destructor has a complete definition of Impl.
LlhttpHttpParserImpl::~LlhttpHttpParserImpl() = default;

int LlhttpHttpParserImpl::execute(const char* slice, int len) { return impl_->execute(slice, len); }

void LlhttpHttpParserImpl::resume() { impl_->resume(); }

int LlhttpHttpParserImpl::pause() { return impl_->pause(); }

int LlhttpHttpParserImpl::getErrno() { return impl_->getErrno(); }

int LlhttpHttpParserImpl::statusCode() const { return impl_->statusCode(); }

int LlhttpHttpParserImpl::httpMajor() const { return impl_->httpMajor(); }

int LlhttpHttpParserImpl::httpMinor() const { return impl_->httpMinor(); }

uint64_t LlhttpHttpParserImpl::contentLength() const { return impl_->contentLength(); }

int LlhttpHttpParserImpl::flags() const { return impl_->flags(); }

uint16_t LlhttpHttpParserImpl::method() const { return impl_->method(); }

const char* LlhttpHttpParserImpl::methodName() const { return impl_->methodName(); }

const char* LlhttpHttpParserImpl::errnoName() {
  return llhttp_errno_name(static_cast<llhttp_errno>(impl_->getErrno()));
}

int LlhttpHttpParserImpl::usesTransferEncoding() const { return impl_->usesTransferEncoding(); }

} // namespace Http1
} // namespace Http
} // namespace Envoy
