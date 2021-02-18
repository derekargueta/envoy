#pragma once

#include <memory>

#include "envoy/common/pure.h"

namespace Envoy {
namespace Http {
namespace Http1 {

/**
 * Every parser implementation should have a corresponding parser type here.
 */
enum class ParserType { Legacy };

enum class MessageType { Request, Response };

// The following define special return values for http_parser callbacks. See:
// https://github.com/nodejs/http-parser/blob/5c5b3ac62662736de9e71640a8dc16da45b32503/http_parser.h#L72
// These codes do not overlap with standard HTTP Status codes. They are only used for user
// callbacks.
enum class HttpParserCode {
  // Callbacks other than on_headers_complete should return a non-zero int to indicate an error
  // and
  // halt execution.
  Error = -1,
  Success = 0,
  // Returning '1' from on_headers_complete will tell http_parser that it should not expect a
  // body.
  NoBody = 1,
  // Returning '2' from on_headers_complete will tell http_parser that it should not expect a body
  // nor any further data on the connection.
  NoBodyData = 2,
};

class ParserCallbacks {
public:
  virtual ~ParserCallbacks() = default;
  /**
   * Called when a request/response is beginning.
   */
   virtual int onMessageBegin() PURE;

   /**
    * Called when URL data is received.
    * @param data supplies the start address.
    * @param length supplies the length.
    */
    virtual int onUrl(const char* data, size_t length) PURE;

    /**
     * Called when header field data is received.
     * @param data supplies the start address.
     * @param length supplies the length.
     */
    virtual int onHeaderField(const char* data, size_t length) PURE;

     /**
      * Called hwen header value data is received.
      * @param data supplies the start address.
      * @param length supplies the length.
      */
    virtual int onHeaderValue(const char* data, size_t length) PURE;

    /**
     * Called when headers are complete.
     * @return 0 if no error, 1 if there should be no body.
     */
    virtual int onHeadersComplete() PURE;

    /**
     * Called when body data is received.
     * @param data supplies the start address.
     * @param length supplies the length
     */
    virtual HttpParserCode bufferBody(const char* data, size_t length) PURE;

    /**
     * Called when the HTTP message has completed parsing.
     */
    virtual int onMessageComplete() PURE;

    virtual int onChunkHeader(bool) PURE;
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
  virtual int usesTransferEncoding() const PURE;
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
