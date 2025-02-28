// RnskiaPackage.java

package abi46_0_0.com.shopify.reactnative.skia;

import java.util.Arrays;
import java.util.Collections;
import java.util.List;

import abi46_0_0.com.facebook.react.ReactPackage;
import abi46_0_0.com.facebook.react.bridge.NativeModule;
import abi46_0_0.com.facebook.react.bridge.ReactApplicationContext;
import abi46_0_0.com.facebook.react.uimanager.ViewManager;

public class RNSkiaPackage implements ReactPackage {
    @Override
    public List<NativeModule> createNativeModules(ReactApplicationContext reactContext) {
        return Arrays.<NativeModule>asList(new RNSkiaModule(reactContext));
    }

    @Override
    public List<ViewManager> createViewManagers(ReactApplicationContext reactContext) {
        return Arrays.<ViewManager>asList(new RNSkiaViewManager());
    }
}
