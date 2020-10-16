#include <string>
#include <iostream>

#include "assert.h"
#include "node_api.h"
#include "v8.h"
#include "v8-profiler.h"

using std::string;
using v8::CpuProfile;
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
  profiler->StartProfiling(GetTitle(env, info));

  return nullptr;
}

napi_value Stop(napi_env env, napi_callback_info info)
{
  napi_status status;

  const CpuProfile *profile = profiler->StopProfiling(GetTitle(env, info));

  napi_value startTime;
  status = napi_create_int64(env, profile->GetStartTime(), &startTime);
  assert(status == napi_ok);

  napi_value endTime;
  status = napi_create_int64(env, profile->GetEndTime(), &endTime);
  assert(status == napi_ok);

  napi_value result;
  status = napi_create_object(env, &result);
  assert(status == napi_ok);
  napi_property_descriptor properties[] = {
      NAPI_VALUE("startTime", startTime),
      NAPI_VALUE("endTime", endTime),
  };
  status = napi_define_properties(env, result, 2, properties);
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