#include "assert.h"
#include "node_api.h"

napi_value Start(napi_env env, napi_callback_info info)
{
  return nullptr;
}

napi_value Stop(napi_env env, napi_callback_info info)
{
  return nullptr;
}

napi_value Init(napi_env env, napi_value exports)
{
  napi_status status;

  napi_property_descriptor properties[] = {
      {"start", 0, Start, 0, 0, 0, napi_default, 0},
      {"stop", 0, Stop, 0, 0, 0, napi_default, 0},
  };

  status = napi_define_properties(env, exports, 2, properties);
  assert(status == napi_ok);

  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)