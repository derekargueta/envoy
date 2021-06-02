#pragma once

#include <string>

#include "envoy/http/header_map.h"
#include "envoy/http/query_params.h"

#include "absl/strings/string_view.h"

namespace Envoy {
namespace Http {

/**
 * Parse a URL into query parameters.
 * @param url supplies the url to parse.
 * @return QueryParams the parsed parameters, if any.
 */
QueryParams parseQueryString(absl::string_view url);

/**
 * Parse a URL into query parameters.
 * @param url supplies the url to parse.
 * @return QueryParams the parsed and percent-decoded parameters, if any.
 */
QueryParams parseAndDecodeQueryString(absl::string_view url);

/**
 * Parse a a request body into query parameters.
 * @param body supplies the body to parse.
 * @return QueryParams the parsed parameters, if any.
 */
QueryParams parseFromBody(absl::string_view body);

/**
 * Parse query parameters from a URL or body.
 * @param data supplies the data to parse.
 * @param start supplies the offset within the data.
 * @param decode_params supplies the flag whether to percent-decode the parsed parameters (both name
 *        and value). Set to false to keep the parameters encoded.
 * @return QueryParams the parsed parameters, if any.
 */
QueryParams parseParameters(absl::string_view data, size_t start, bool decode_params);

/**
 * Finds the start of the query string in a path
 * @param path supplies a HeaderString& to search for the query string
 * @return absl::string_view starting at the beginning of the query string,
 *         or a string_view starting at the end of the path if there was
 *         no query string.
 */
absl::string_view findQueryStringStart(const HeaderString& path);

/**
 * Serialize query-params into a string.
 */
std::string queryParamsToString(const QueryParams& query_params);

} // namespace Http
} // namespace Envoy
