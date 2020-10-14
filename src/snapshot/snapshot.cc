#include "assert.h"
#include "node_api.h"
#include "v8.h"

napi_value Method(napi_env env, napi_callback_info info)
{
  napi_status status;

  
}

napi_value Init(napi_env env, napi_value exports)
{
  napi_status status;
  napi_property_descriptor desc = {"snapshot", 0, Method, 0, 0, 0, napi_default, 0};
  status = napi_define_properties(env, exports, 1, &desc);
  assert(status == napi_ok);
  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)