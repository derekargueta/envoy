#include "common/http/query_params.h"

#include "gtest/gtest.h"

namespace Envoy {
namespace Http {

TEST(QueryParams, parseQueryString) {
  EXPECT_EQ(QueryParams(), parseQueryString("/hello"));
  EXPECT_EQ(QueryParams(), parseAndDecodeQueryString("/hello"));

  EXPECT_EQ(QueryParams(), parseQueryString("/hello?"));
  EXPECT_EQ(QueryParams(), parseAndDecodeQueryString("/hello?"));

  EXPECT_EQ(QueryParams({{"hello", ""}}), parseQueryString("/hello?hello"));
  EXPECT_EQ(QueryParams({{"hello", ""}}),
            parseAndDecodeQueryString("/hello?hello"));

  EXPECT_EQ(QueryParams({{"hello", "world"}}),
            parseQueryString("/hello?hello=world"));
  EXPECT_EQ(QueryParams({{"hello", "world"}}),
            parseAndDecodeQueryString("/hello?hello=world"));

  EXPECT_EQ(QueryParams({{"hello", ""}}), parseQueryString("/hello?hello="));
  EXPECT_EQ(QueryParams({{"hello", ""}}),
            parseAndDecodeQueryString("/hello?hello="));

  EXPECT_EQ(QueryParams({{"hello", ""}}), parseQueryString("/hello?hello=&"));
  EXPECT_EQ(QueryParams({{"hello", ""}}),
            parseAndDecodeQueryString("/hello?hello=&"));

  EXPECT_EQ(QueryParams({{"hello", ""}, {"hello2", "world2"}}),
            parseQueryString("/hello?hello=&hello2=world2"));
  EXPECT_EQ(QueryParams({{"hello", ""}, {"hello2", "world2"}}),
            parseAndDecodeQueryString("/hello?hello=&hello2=world2"));

  EXPECT_EQ(QueryParams({{"name", "admin"}, {"level", "trace"}}),
            parseQueryString("/logging?name=admin&level=trace"));
  EXPECT_EQ(QueryParams({{"name", "admin"}, {"level", "trace"}}),
            parseAndDecodeQueryString("/logging?name=admin&level=trace"));

  EXPECT_EQ(QueryParams({{"param_value_has_encoded_ampersand", "a%26b"}}),
            parseQueryString("/hello?param_value_has_encoded_ampersand=a%26b"));
  EXPECT_EQ(QueryParams({{"param_value_has_encoded_ampersand", "a&b"}}),
            parseAndDecodeQueryString("/hello?param_value_has_encoded_ampersand=a%26b"));

  EXPECT_EQ(QueryParams({{"params_has_encoded_%26", "a%26b"}, {"ok", "1"}}),
            parseQueryString("/hello?params_has_encoded_%26=a%26b&ok=1"));
  EXPECT_EQ(QueryParams({{"params_has_encoded_&", "a&b"}, {"ok", "1"}}),
            parseAndDecodeQueryString("/hello?params_has_encoded_%26=a%26b&ok=1"));

  // A sample of request path with query strings by Prometheus:
  // https://github.com/envoyproxy/envoy/issues/10926#issuecomment-651085261.
  EXPECT_EQ(
      QueryParams(
          {{"filter",
            "%28cluster.upstream_%28rq_total%7Crq_time_sum%7Crq_time_count%7Crq_time_"
            "bucket%7Crq_xx%7Crq_complete%7Crq_active%7Ccx_active%29%29%7C%28server.version%29"}}),
      parseQueryString(
          "/stats?filter=%28cluster.upstream_%28rq_total%7Crq_time_sum%7Crq_time_count%7Crq_time_"
          "bucket%7Crq_xx%7Crq_complete%7Crq_active%7Ccx_active%29%29%7C%28server.version%29"));
  EXPECT_EQ(
      QueryParams(
          {{"filter", "(cluster.upstream_(rq_total|rq_time_sum|rq_time_count|rq_time_bucket|rq_xx|"
                      "rq_complete|rq_active|cx_active))|(server.version)"}}),
      parseAndDecodeQueryString(
          "/stats?filter=%28cluster.upstream_%28rq_total%7Crq_time_sum%7Crq_time_count%7Crq_time_"
          "bucket%7Crq_xx%7Crq_complete%7Crq_active%7Ccx_active%29%29%7C%28server.version%29"));
}

TEST(QueryParams, QueryParamsToString) {
  EXPECT_EQ("", queryParamsToString(QueryParams({})));
  EXPECT_EQ("?a=1", queryParamsToString(QueryParams({{"a", "1"}})));
  EXPECT_EQ("?a=1&b=2",
            queryParamsToString(QueryParams({{"a", "1"}, {"b", "2"}})));
}

} // namespace Http
} // namespace Envoy
