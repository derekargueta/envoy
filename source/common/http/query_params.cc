#include "common/http/query_params.h"

#include <string>

#include "common/common/utility.h"
#include "common/http/utility.h"

namespace Envoy {
namespace Http {

QueryParams parseQueryString(absl::string_view url) {
  size_t start = url.find('?');
  if (start == std::string::npos) {
    QueryParams params;
    return params;
  }

  start++;
  return parseParameters(url, start, /*decode_params=*/false);
}

QueryParams parseAndDecodeQueryString(absl::string_view url) {
  size_t start = url.find('?');
  if (start == std::string::npos) {
    QueryParams params;
    return params;
  }

  start++;
  return parseParameters(url, start, /*decode_params=*/true);
}

QueryParams parseFromBody(absl::string_view body) {
  return parseParameters(body, 0, /*decode_params=*/true);
}

QueryParams parseParameters(absl::string_view data, size_t start,
                                              bool decode_params) {
  QueryParams params;

  while (start < data.size()) {
    size_t end = data.find('&', start);
    if (end == std::string::npos) {
      end = data.size();
    }
    absl::string_view param(data.data() + start, end - start);

    const size_t equal = param.find('=');
    if (equal != std::string::npos) {
      const auto param_name = StringUtil::subspan(data, start, start + equal);
      const auto param_value = StringUtil::subspan(data, start + equal + 1, end);
      params.emplace(decode_params ? Utility::PercentEncoding::decode(param_name) : param_name,
                     decode_params ? Utility::PercentEncoding::decode(param_value) : param_value);
    } else {
      params.emplace(StringUtil::subspan(data, start, end), "");
    }

    start = end + 1;
  }

  return params;
}

absl::string_view findQueryStringStart(const HeaderString& path) {
  absl::string_view path_str = path.getStringView();
  size_t query_offset = path_str.find('?');
  if (query_offset == absl::string_view::npos) {
    query_offset = path_str.length();
  }
  path_str.remove_prefix(query_offset);
  return path_str;
}

// TODO(jmarantz): make QueryParams a real class and put this serializer there,
// along with proper URL escaping of the name and value.
std::string queryParamsToString(const QueryParams& params) {
  std::string out;
  std::string delim = "?";
  for (const auto& p : params) {
    absl::StrAppend(&out, delim, p.first, "=", p.second);
    delim = "&";
  }
  return out;
}

} // namespace Http
} // namespace Envoy
