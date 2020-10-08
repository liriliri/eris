#include <chrono>
#include <thread>

#include "assert.h"
#include "node_api.h"

napi_value Method(napi_env env, napi_callback_info info)
{
  napi_status status;
  napi_value undefined;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
  assert(status == napi_ok);

  int64_t milliseconds;
  status = napi_get_value_int64(env, args[0], &milliseconds);
  assert(status == napi_ok);

  std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));

  status = napi_get_undefined(env, &undefined);
  assert(status == napi_ok);

  return undefined;
}

napi_value Init(napi_env env, napi_value exports)
{
  napi_status status;
  napi_property_descriptor desc = {"sleep", 0, Method, 0, 0, 0, napi_default, 0};
  status = napi_define_properties(env, exports, 1, &desc);
  assert(status == napi_ok);
  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)