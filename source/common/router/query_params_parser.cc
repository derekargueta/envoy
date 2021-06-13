#include "source/common/router/query_params_parser.h"

namespace Envoy {
namespace Router {

QueryParamsParserPtr QueryParamsParser::configure(const Protobuf::RepeatedPtrField<std::string>&) {
  QueryParamsParserPtr query_params_parser(new QueryParamsParser());

  return query_params_parser;
}

void QueryParamsParser::evaluateQueryParams(Http::HeaderMap&) {
  // for (const auto& : query_params_to_remove_) {
    // query_params.remove(query_param);
  // }
}

} // namespace Router
} // namespace Envoy
