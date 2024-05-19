#pragma once

#include <memory>

#include "envoy/extensions/filters/http/query_parameter_mutation/v3/config.pb.h"

#include "envoy/http/filter.h"
#include "envoy/http/header_map.h"
#include "envoy/router/router.h"
#include "envoy/server/factory_context.h"

#include "source/common/router/query_params_evaluator.h"
#include "source/extensions/filters/http/common/pass_through_filter.h"

namespace Envoy {
namespace Extensions {
namespace HttpFilters {
namespace QueryParameterMutation {

class Config : public Router::RouteSpecificFilterConfig {
public:
  Config(const envoy::extensions::filters::http::query_parameter_mutation::v3::Config& proto_config,
         Server::Configuration::CommonFactoryContext&);

  void evaluateQueryParams(Http::RequestHeaderMap& headers) const;

private:
  Router::QueryParamsEvaluatorPtr query_params_evaluator_;
};

using ConfigSharedPtr = std::shared_ptr<Config>;

class Filter : public Http::PassThroughDecoderFilter {
public:
  Filter(ConfigSharedPtr config);

  Http::FilterHeadersStatus decodeHeaders(Http::RequestHeaderMap& headers, bool) override;

private:
  ConfigSharedPtr config_;
  absl::InlinedVector<const Config*, 3> route_configs_{};
};

} // namespace QueryParameterMutation
} // namespace HttpFilters
} // namespace Extensions
} // namespace Envoy
