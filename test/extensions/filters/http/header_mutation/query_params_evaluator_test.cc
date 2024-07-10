#include <memory>
#include <string>

#include "source/common/router/string_accessor_impl.h"
#include "source/extensions/filters/http/header_mutation/query_params_evaluator.h"

#include "test/mocks/stream_info/mocks.h"
#include "test/test_common/utility.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace Envoy {
namespace Extensions {
namespace HttpFilters {
namespace HeaderMutation {

class QueryParamsEvaluatorTest : public testing::Test {
public:
  std::string evaluateWithPath(const std::string& path) {
    auto paramsEvaluator = std::make_unique<QueryParamsEvaluator>(mutations_);
    Http::TestRequestHeaderMapImpl request_headers{{":path", path}};
    paramsEvaluator->evaluateQueryParams(request_headers, stream_info_);
    return std::string(request_headers.getPathValue());
  }

  void addParamToAdd(absl::string_view key, absl::string_view value, AppendAction append_action) {
    auto* qp = envoy::config::core::v3::KeyValue::default_instance().New();
    qp->set_key(key);
    qp->set_value(value);

    auto* vo = KeyValueAppendProto::default_instance().New();
    vo->set_action(static_cast<KeyValueAppendActionProto>(append_action));
    vo->set_allocated_entry(qp);

    auto* mutation = mutations_.Add();
    mutation->set_allocated_append(vo);
  }

  void addParamToRemove(absl::string_view key) {
    auto* mutation = mutations_.Add();
    mutation->set_remove(key);
  }

  void setFilterData(absl::string_view key, absl::string_view value) {
    stream_info_.filter_state_->setData(key, std::make_unique<Router::StringAccessorImpl>(value),
                                        StreamInfo::FilterState::StateType::ReadOnly,
                                        StreamInfo::FilterState::LifeSpan::FilterChain);
  }

  Protobuf::RepeatedPtrField<KeyValueMutationProto> mutations_;
  testing::NiceMock<StreamInfo::MockStreamInfo> stream_info_;
};

TEST_F(QueryParamsEvaluatorTest, EmptyConfigEvaluator) {
  const auto old_path = "/path?this=should&stay=the&exact=same";
  const auto new_path = evaluateWithPath(old_path);
  EXPECT_EQ("/path?this=should&stay=the&exact=same", new_path);
}

TEST_F(QueryParamsEvaluatorTest, AppendIfExistsOrAddWhenAbsent) {
  addParamToAdd("foo", "value_new", AppendAction::AppendIfExistsOrAdd);

  const auto old_path = "/path";
  const auto new_path = evaluateWithPath(old_path);
  EXPECT_EQ("/path?foo=value_new", new_path);
}

TEST_F(QueryParamsEvaluatorTest, AppendIfExistsOrAddWhenPresent) {
  addParamToAdd("foo", "value_new", AppendAction::AppendIfExistsOrAdd);

  const auto old_path = "/path?foo=value_old";
  const auto new_path = evaluateWithPath(old_path);
  EXPECT_EQ("/path?foo=value_old&foo=value_new", new_path);
}

TEST_F(QueryParamsEvaluatorTest, AddIfAbsentWhenAbsent) {
  addParamToAdd("foo", "value", AppendAction::AddIfAbsent);

  const auto old_path = "/path?";
  const auto new_path = evaluateWithPath(old_path);
  EXPECT_EQ("/path?foo=value", new_path);
}

TEST_F(QueryParamsEvaluatorTest, AddIfAbsentWhenPresent) {
  addParamToAdd("foo", "value_new", AppendAction::AddIfAbsent);

  const auto old_path = "/path?foo=value_old";
  const auto new_path = evaluateWithPath(old_path);
  EXPECT_EQ("/path?foo=value_old", new_path);
}

TEST_F(QueryParamsEvaluatorTest, OverWriteIfExistsOrAddWhenAbsent) {
  addParamToAdd("foo", "value_new", AppendAction::OverwriteIfExistsOrAdd);

  const auto old_path = "/path";
  const auto new_path = evaluateWithPath(old_path);
  EXPECT_EQ("/path?foo=value_new", new_path);
}

TEST_F(QueryParamsEvaluatorTest, OverWriteIfExistsOrAddWhenPresent) {
  addParamToAdd("foo", "value_new", AppendAction::OverwriteIfExistsOrAdd);

  const auto old_path = "/path?foo=value_old";
  const auto new_path = evaluateWithPath(old_path);
  EXPECT_EQ("/path?foo=value_new", new_path);
}

TEST_F(QueryParamsEvaluatorTest, OverWriteIfExistsWhenAbsent) {
  addParamToAdd("foo", "value_new", AppendAction::OverwriteIfExists);

  const auto old_path = "/path";
  const auto new_path = evaluateWithPath(old_path);
  EXPECT_EQ("/path", new_path);
}

TEST_F(QueryParamsEvaluatorTest, OverWriteIfExistsWhenPresent) {
  addParamToAdd("foo", "value_new", AppendAction::OverwriteIfExists);

  const auto old_path = "/path?foo=value_old";
  const auto new_path = evaluateWithPath(old_path);
  EXPECT_EQ("/path?foo=value_new", new_path);
}

TEST_F(QueryParamsEvaluatorTest, ChainMultipleParams) {
  addParamToAdd("foo", "value_1", AppendAction::AppendIfExistsOrAdd);
  addParamToAdd("foo", "value_2", AppendAction::AppendIfExistsOrAdd);

  const auto old_path = "/path?bar=123";
  const auto new_path = evaluateWithPath(old_path);
  EXPECT_EQ("/path?bar=123&foo=value_1&foo=value_2", new_path);
}

TEST_F(QueryParamsEvaluatorTest, RemoveMultipleParams) {
  addParamToRemove("foo");

  const auto old_path = "/path?foo=value_1&foo=value_2&bar=123";
  const auto new_path = evaluateWithPath(old_path);
  EXPECT_EQ("/path?bar=123", new_path);
}

TEST_F(QueryParamsEvaluatorTest, AddEmptyValue) {
  addParamToAdd("foo", "", AppendAction::AppendIfExistsOrAdd);

  const auto old_path = "/path?bar=123";
  const auto new_path = evaluateWithPath(old_path);
  EXPECT_EQ("/path?bar=123&foo=", new_path);
}

TEST_F(QueryParamsEvaluatorTest, CommandSubstitution) {
  addParamToAdd("start_time", "%START_TIME(%s)%", AppendAction::AppendIfExistsOrAdd);
  setFilterData("variable", "substituted-value");

  const SystemTime start_time(std::chrono::seconds(1522796769));
  EXPECT_CALL(stream_info_, startTime()).WillRepeatedly(testing::Return(start_time));

  const auto old_path = "/path";
  const auto new_path = evaluateWithPath(old_path);
  EXPECT_EQ("/path?start_time=1522796769", new_path);
}

TEST_F(QueryParamsEvaluatorTest, CommandSubstitutionFilterState) {
  addParamToAdd("foo", "%FILTER_STATE(variable)%", AppendAction::AppendIfExistsOrAdd);
  setFilterData("variable", "substituted-value");

  const auto old_path = "/path?bar=123";
  const auto new_path = evaluateWithPath(old_path);
  EXPECT_EQ("/path?bar=123&foo=\"substituted-value\"", new_path);
}

} // namespace HeaderMutation
} // namespace HttpFilters
} // namespace Extensions
} // namespace Envoy
