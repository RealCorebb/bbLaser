#include <stdlib.h>
#include <stdlib.h>  /* abs */
#include "nan.h"

using namespace node;

namespace {

NAN_METHOD(ArrayAbs) {
  int *arr = reinterpret_cast<int *>(Buffer::Data(info[0].As<v8::Object>()));
  uint32_t length = info[1]->Uint32Value();
  for (uint32_t i = 0; i < length; i++) {
    *(arr + i) = abs(arr[i]);
  }
}

void Initialize(v8::Handle<v8::Object> target) {
  Nan::HandleScope scope;
  Nan::SetMethod(target, "arrayAbs", ArrayAbs);
}

} // anonymous namespace

NODE_MODULE(native_tests, Initialize);
