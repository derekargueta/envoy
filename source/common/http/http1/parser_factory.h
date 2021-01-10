#pragma once

#include <memory>

#include "common/common/assert.h"
#include "common/http/http1/legacy_parser.h"
#include "common/http/http1/llhttp_parser.h"

namespace Envoy {
namespace Http {
namespace Http1 {

class ParserFactory {
public:
  static ParserPtr createParser(ParserType parser_type, MessageType message_type, ParserCallbacks* cb) {
    switch (parser_type) {
    case ParserType::Legacy:
      return std::make_unique<LegacyHttpParserImpl>(message_type, cb);
    case ParserType::Llhttp:
      return std::make_unique<LlhttpHttpParserImpl>(message_type, cb);
    default:
      NOT_IMPLEMENTED_GCOVR_EXCL_LINE;
    }
  }
};

} // namespace Http1
} // namespace Http
} // namespace Envoy
