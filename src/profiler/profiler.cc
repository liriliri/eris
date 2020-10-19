#include <string>
#include <vector>

#include "assert.h"
#include "nan.h"
#include "node_api.h"
#include "v8.h"
#include "v8-profiler.h"

using std::string;
using std::vector;
using v8::CpuProfile;
using v8::CpuProfileNode;
using v8::CpuProfiler;
using v8::Isolate;
using v8::Local;
using v8::String;

#define NAPI_METHOD(name, func)                \
  {                                            \
    name, 0, func, 0, 0, 0, napi_enumerable, 0 \
  }

#define NAPI_VALUE(name, value)                 \
  {                                             \
    name, 0, 0, 0, 0, value, napi_enumerable, 0 \
  }

CpuProfiler *profiler = CpuProfiler::New(Isolate::GetCurrent());

string napi_value_to_string(napi_env env, napi_value &value)
{
  string ret;
  size_t length;
  napi_get_value_string_utf8(env, value, nullptr, 0, &length);
  ret.reserve(length + 1);
  ret.resize(length);
  napi_get_value_string_utf8(env, value, &ret[0], ret.capacity(), nullptr);

  return ret;
}

Local<String> GetTitle(napi_env env, napi_callback_info info)
{
  size_t argc = 1;
  napi_value args[1];
  napi_status status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
  assert(status == napi_ok);
  string title = napi_value_to_string(env, args[0]);
  return String::NewFromUtf8(Isolate::GetCurrent(), title.c_str()).ToLocalChecked();
}

napi_value Start(napi_env env, napi_callback_info info)
{
  profiler->StartProfiling(GetTitle(env, info), true);

  return nullptr;
}

napi_value buildCallFrame(napi_env env, const CpuProfileNode *node)
{
  napi_status status;
  napi_value callFrame;

  status = napi_create_object(env, &callFrame);
  assert(status == napi_ok);

  napi_value functionName;
  string name(*Nan::Utf8String(node->GetFunctionName()));
  status = napi_create_string_utf8(env, &name[0], name.length(), &functionName);
  assert(status == napi_ok);

  napi_value url;
  string resourceName(*Nan::Utf8String(node->GetScriptResourceName()));
  status = napi_create_string_utf8(env, &resourceName[0], resourceName.length(), &url);
  assert(status == napi_ok);

  napi_value lineNumber;
  status = napi_create_int32(env, node->GetLineNumber() - 1, &lineNumber);
  assert(status == napi_ok);

  napi_value columnNumber;
  status = napi_create_int32(env, node->GetColumnNumber() - 1, &columnNumber);
  assert(status == napi_ok);

  napi_property_descriptor properties[] = {
      NAPI_VALUE("functionName", functionName),
      NAPI_VALUE("url", url),
      NAPI_VALUE("lineNumber", lineNumber),
      NAPI_VALUE("columnNumber", columnNumber),
  };
  status = napi_define_properties(env, callFrame, 3, properties);
  assert(status == napi_ok);

  return callFrame;
}

napi_value buildNode(napi_env env, const CpuProfileNode *node)
{
  napi_status status;
  napi_value result;

  status = napi_create_object(env, &result);
  assert(status == napi_ok);

  napi_value id;
  status = napi_create_uint32(env, node->GetNodeId(), &id);
  assert(status == napi_ok);

  napi_value hitCount;
  status = napi_create_uint32(env, node->GetHitCount(), &hitCount);
  assert(status == napi_ok);

  napi_property_descriptor properties[] = {
      NAPI_VALUE("id", id),
      NAPI_VALUE("callFrame", buildCallFrame(env, node)),
      NAPI_VALUE("hitCount", hitCount),
  };
  status = napi_define_properties(env, result, 2, properties);
  assert(status == napi_ok);

  const int childrenCount = node->GetChildrenCount();
  if (childrenCount)
  {
    napi_value children;
    status = napi_create_array_with_length(env, childrenCount, &children);
    for (int i = 0; i < childrenCount; i++)
    {
      napi_value id;
      status = napi_create_uint32(env, node->GetChild(i)->GetNodeId(), &id);
      assert(status == napi_ok);
      status = napi_set_element(env, children, i, id);
      assert(status == napi_ok);
    }
    napi_property_descriptor properties[] = {
        NAPI_VALUE("children", children),
    };
    status = napi_define_properties(env, result, 1, properties);
    assert(status == napi_ok);
  }

  return result;
}

void flattenNodesTree(napi_env env, const CpuProfileNode *node, vector<napi_value> *list)
{
  list->push_back(buildNode(env, node));
  const int childrenCount = node->GetChildrenCount();
  for (int i = 0; i < childrenCount; i++)
    flattenNodesTree(env, node->GetChild(i), list);
}

napi_value Stop(napi_env env, napi_callback_info info)
{
  napi_status status;

  const CpuProfile *profile = profiler->StopProfiling(GetTitle(env, info));

  const CpuProfileNode *root = profile->GetTopDownRoot();
  vector<napi_value> list;
  flattenNodesTree(env, root, &list);
  napi_value nodes;
  status = napi_create_array_with_length(env, list.size(), &nodes);
  assert(status == napi_ok);
  for (int i = 0; i < list.size(); i++)
  {
    status = napi_set_element(env, nodes, i, list[i]);
    assert(status == napi_ok);
  }

  napi_value startTime;
  status = napi_create_int64(env, profile->GetStartTime(), &startTime);
  assert(status == napi_ok);

  napi_value endTime;
  status = napi_create_int64(env, profile->GetEndTime(), &endTime);
  assert(status == napi_ok);

  napi_value samples;
  int count = profile->GetSamplesCount();
  status = napi_create_array_with_length(env, count, &samples);
  for (int i = 0; i < count; i++)
  {
    unsigned int nodeId = profile->GetSample(i)->GetNodeId();
    napi_value sample;
    status = napi_create_uint32(env, nodeId, &sample);
    assert(status == napi_ok);
    status = napi_set_element(env, samples, i, sample);
    assert(status == napi_ok);
  }
  assert(status == napi_ok);

  napi_value timeDeltas;
  status = napi_create_array_with_length(env, count, &timeDeltas);
  assert(status == napi_ok);
  uint64_t lastTime = profile->GetStartTime();
  for (int i = 0; i < count; i++)
  {
    uint64_t ts = profile->GetSampleTimestamp(i);
    napi_value timestamp;
    status = napi_create_int64(env, ts - lastTime, &timestamp);
    assert(status == napi_ok);
    status = napi_set_element(env, timeDeltas, i, timestamp);
    assert(status == napi_ok);
    lastTime = ts;
  }

  napi_value result;
  status = napi_create_object(env, &result);
  assert(status == napi_ok);
  napi_property_descriptor properties[] = {
      NAPI_VALUE("nodes", nodes),
      NAPI_VALUE("startTime", startTime),
      NAPI_VALUE("endTime", endTime),
      NAPI_VALUE("samples", samples),
      NAPI_VALUE("timeDeltas", timeDeltas),
  };
  status = napi_define_properties(env, result, 5, properties);
  assert(status == napi_ok);

  return result;
}

napi_value Init(napi_env env, napi_value exports)
{
  napi_status status;

  napi_property_descriptor properties[] = {
      NAPI_METHOD("start", Start),
      NAPI_METHOD("stop", Stop),
  };

  status = napi_define_properties(env, exports, 2, properties);
  assert(status == napi_ok);

  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)