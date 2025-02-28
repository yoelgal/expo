#pragma once

#include <memory>
#include <utility>

#include <ABI46_0_0jsi/ABI46_0_0jsi.h>
#include <ABI46_0_0ReactCommon/ABI46_0_0TurboModuleUtils.h>

#include "JsiSkData.h"


namespace ABI46_0_0RNSkia {

    using namespace ABI46_0_0facebook;

    class JsiSkDataFactory : public JsiSkHostObject {
    public:
        JSI_HOST_FUNCTION(fromURI) {
            auto jsiLocalUri = arguments[0].asString(runtime);
            auto localUri = jsiLocalUri.utf8(runtime);
            auto context = getContext();
            return ABI46_0_0React::createPromiseAsJSIValue(
                    runtime,
                    [context = std::move(context), localUri = std::move(localUri)](jsi::Runtime &runtime,
                                        std::shared_ptr<ABI46_0_0React::Promise> promise) -> void {
                        // Create a stream operation - this will be run in a
                        // separate thread
                        context->performStreamOperation(
                                localUri,
                                [&runtime, context = std::move(context), promise = std::move(promise)](
                                        std::unique_ptr<SkStreamAsset> stream) -> void {
                                    // Schedule drawCallback on the Javascript thread
                                    auto result = SkData::MakeFromStream(stream.get(), stream->getLength());
                                    context->runOnJavascriptThread([&runtime, context = std::move(context), promise = std::move(promise),
                                                                           result = std::move(result)]() {
                                        promise->resolve(jsi::Object::createFromHostObject(
                                                runtime,
                                                std::make_shared<JsiSkData>(std::move(context),
                                                                            std::move(result))));
                                    });
                        });
            });
        };

        JSI_HOST_FUNCTION(fromBytes) {
            auto array = arguments[0].asObject(runtime);
            jsi::ArrayBuffer buffer = array
                    .getProperty(runtime, jsi::PropNameID::forAscii(runtime, "buffer"))
                    .asObject(runtime)
                    .getArrayBuffer(runtime);

            auto data = SkData::MakeWithCopy(buffer.data(runtime), buffer.size(runtime));
            return jsi::Object::createFromHostObject(runtime,
                                                     std::make_shared<JsiSkData>(
                                                             getContext(), std::move(data)));
        }

        JSI_HOST_FUNCTION(fromBase64) {
            auto base64 = arguments[0].asString(runtime);
            auto base64Str = base64.utf8(runtime);
            auto size = base64Str.size();

            // Calculate length
            size_t len;
            auto err = SkBase64::Decode(&base64.utf8(runtime).c_str()[0], size, nullptr, &len);
            if(err != SkBase64::Error::kNoError) {
              jsi::detail::throwJSError(runtime, "Error decoding base64 string");
              return jsi::Value::undefined();
            }

            // Create data object and decode
            auto data = SkData::MakeUninitialized(len);
            err = SkBase64::Decode(&base64.utf8(runtime).c_str()[0], size, data->writable_data(), &len);
            if(err != SkBase64::Error::kNoError) {
              jsi::detail::throwJSError(runtime, "Error decoding base64 string");
              return jsi::Value::undefined();
            }
          
            return jsi::Object::createFromHostObject(runtime,
                                                     std::make_shared<JsiSkData>(
                                                             getContext(), std::move(data)));
        }

        JSI_EXPORT_FUNCTIONS(JSI_EXPORT_FUNC(JsiSkDataFactory, fromURI),
                             JSI_EXPORT_FUNC(JsiSkDataFactory, fromBytes),
                             JSI_EXPORT_FUNC(JsiSkDataFactory, fromBase64))

        JsiSkDataFactory(std::shared_ptr<ABI46_0_0RNSkPlatformContext> context)
                : JsiSkHostObject(std::move(context)) {}
    };

} // namespace ABI46_0_0RNSkia
