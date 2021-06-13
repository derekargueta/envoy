#pragma once

#include <memory>
#include <string>
#include <vector>

#include "envoy/http/header_map.h"

#include "source/common/protobuf/protobuf.h"

namespace Envoy {
namespace Router {

class QueryParamsParser;
using QueryParamsParserPtr = std::unique_ptr<QueryParamsParser>;

class QueryParamsParser {
public:
  static QueryParamsParserPtr configure(const Protobuf::RepeatedPtrField<std::string>& query_params_to_remove);

  void evaluateQueryParams(Http::HeaderMap& headers);

private:
  std::vector<std::string> query_params_to_remove_;
};

} // namespace Router
} // namespace Envoy
