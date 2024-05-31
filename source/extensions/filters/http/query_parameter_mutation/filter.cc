#include "source/extensions/filters/http/query_parameter_mutation/filter.h"

#include "envoy/extensions/filters/http/query_parameter_mutation/v3/config.pb.h"
#include "envoy/http/filter.h"
#include "envoy/http/header_map.h"
#include "envoy/server/factory_context.h"

#include "source/common/http/utility.h"
#include "source/extensions/filters/http/query_parameter_mutation/query_params_evaluator.h"

namespace Envoy {
namespace Extensions {
namespace HttpFilters {
namespace QueryParameterMutation {

Config::Config(const FilterConfigProto& config)
    : query_params_evaluator_(std::make_unique<QueryParamsEvaluator>(
          config.query_parameters_to_add(), config.query_parameters_to_remove())) {}

void Config::evaluateQueryParams(Http::RequestHeaderMap& headers,
                                 StreamInfo::StreamInfo& stream_info) const {
  query_params_evaluator_->evaluateQueryParams(headers, stream_info);
}

Filter::Filter(ConfigSharedPtr config) : config_(config) {}

Http::FilterHeadersStatus Filter::decodeHeaders(Http::RequestHeaderMap& headers, bool) {
  const auto route_configs = Http::Utility::getAllPerFilterConfig<Config>(decoder_callbacks_);

  auto& stream_info = decoder_callbacks_->streamInfo();
  config_->evaluateQueryParams(headers, stream_info);

  const auto& virtual_host = decoder_callbacks_->route()->routeEntry()->virtualHost();
  if (virtual_host.routeConfig().mostSpecificHeaderMutationsWins()) {
    for (auto config : route_configs) {
      config->evaluateQueryParams(headers, stream_info);
    }
  } else {
    for (auto it = route_configs.rbegin(); it != route_configs.rend(); ++it) {
      (*it)->evaluateQueryParams(headers, stream_info);
    }
  }

  return Http::FilterHeadersStatus::Continue;
}

} // namespace QueryParameterMutation
} // namespace HttpFilters
} // namespace Extensions
} // namespace Envoy
