#pragma once

#include <memory>

#include "envoy/common/pure.h"

namespace Envoy {
namespace Http {
namespace Http1 {

enum class ParserType { Legacy };

enum class MessageType { Request, Response };

class ParserCallbacks {
public:
  virtual ~ParserCallbacks() = default;
  /**
   * Called when a request/response is beginning.
   */
   virtual void onMessageBeginBase() PURE;

   /**
    * Called when URL data is received.
    * @param data supplies the start address.
    * @param length supplies the length.
    */
    virtual void onUrl(const char* data, size_t length) PURE;

    /**
     * Called when header field data is received.
     * @param data supplies the start address.
     * @param length supplies the length.
     */
    virtual void onHeaderField(const char* data, size_t length) PURE;

     /**
      * Called hwen header value data is received.
      * @param data supplies the start address.
      * @param length supplies the length.
      */
    virtual void onHeaderValue(const char* data, size_t length) PURE;

    /**
     * Called when headers are complete.
     * @return 0 if no error, 1 if there should be no body.
     */
    virtual int onHeadersCompleteBase() PURE;

    /**
     * Called when body data is received.
     * @param data supplies the start address.
     * @param length supplies the length
     */
    virtual void bufferBody(const char* data, size_t length) PURE;

    /**
     * Called when the HTTP message has completed parsing.
     */
    virtual void onMessageCompleteBase() PURE;
    virtual void onChunkHeader(bool) PURE;
};

class Parser {
public:
  virtual ~Parser() = default;
  virtual int execute(const char* slice, int len) PURE;
  virtual void resume() PURE;
  virtual int pause() PURE;
  virtual int getErrno() PURE;
  virtual int statusCode() const PURE;
  virtual int httpMajor() const PURE;
  virtual int httpMinor() const PURE;
  virtual uint64_t contentLength() const PURE;
  virtual int flags() const PURE;
  virtual uint16_t method() const PURE;
  virtual const char* methodName() const PURE;
  virtual const char* errnoName() PURE;
};

enum class Flags {
  Chunked = 1,
};

enum class ParserStatus {
  Ok = 0,
  Paused = 31,
};

enum class Method {
  Head = 2,
  Connect = 5,
  Options = 6,
};

using ParserPtr = std::unique_ptr<Parser>;

} // namespace Http1
} // namespace Http
} // namespace Envoy
