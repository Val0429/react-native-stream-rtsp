
package com.isap;

import com.facebook.react.bridge.ReactApplicationContext;
import com.facebook.react.bridge.ReactContextBaseJavaModule;
import com.facebook.react.bridge.ReactMethod;
import com.facebook.react.bridge.Callback;

public class RNStreamRtspModule extends ReactContextBaseJavaModule {

  private final ReactApplicationContext reactContext;

  public RNStreamRtspModule(ReactApplicationContext reactContext) {
    super(reactContext);
    this.reactContext = reactContext;

    MainApplication.setCurrentActivity(this.getCurrentActivity());
  }

  @Override
  public String getName() {
    return "RNStreamRtsp";
  }
}