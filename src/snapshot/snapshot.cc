#include <string>

#include "assert.h"
#include "node_api.h"
#include "v8.h"
#include "v8-profiler.h"

using std::string;

class OutputStream : public v8::OutputStream
{
public:
  string str;
  OutputStream() {}
  void EndOfStream() {}
  WriteResult WriteAsciiChunk(char *data, int size)
  {
    str.append(data, size);
    return kContinue;
  }
};

napi_value Method(napi_env env, napi_callback_info info)
{
  napi_status status;
  napi_value result;

  v8::Isolate *const isolate = v8::Isolate::GetCurrent();
  const v8::HeapSnapshot *const snapshot = isolate->GetHeapProfiler()->TakeHeapSnapshot();

  OutputStream stream;
  snapshot->Serialize(&stream);
  const_cast<v8::HeapSnapshot *>(snapshot)->Delete();

  string str = stream.str;
  status = napi_create_string_utf8(env, &str[0], str.length(), &result);
  assert(status == napi_ok);

  return result;
}

napi_value Init(napi_env env, napi_value exports)
{
  napi_value new_exports;

  napi_status status = napi_create_function(env, "snapshot", NAPI_AUTO_LENGTH, Method, nullptr, &new_exports);
  assert(status == napi_ok);

  return new_exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)