#pragma once

#include <memory>

#include "common/http/http1/parser.h"

namespace Envoy {
namespace Http {
namespace Http1 {

class LegacyHttpParserImpl : public Parser {
public:
  LegacyHttpParserImpl(MessageType type, ParserCallbacks* data);
  ~LegacyHttpParserImpl();

  // Http1::Parser
  int execute(const char* data, int len) override;
  void resume() override;
  int pause() override;
  int getErrno() override;
  int statusCode() const override;
  int httpMajor() const override;
  int httpMinor() const override;
  uint64_t contentLength() const override;
  int flags() const override;
  uint16_t method() const override;
  const char* methodName() const override;
  const char* errnoName() override;
  int usesTransferEncoding() const override;

private:
  class Impl;
  std::unique_ptr<Impl> impl_;
};

} // namespace Http1
} // namespace Http
} // namespace Envoy
