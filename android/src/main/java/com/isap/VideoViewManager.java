package com.isap;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.support.annotation.Nullable;
import android.util.Log;

import com.facebook.drawee.controller.AbstractDraweeControllerBuilder;
import com.facebook.react.bridge.Promise;
import com.facebook.react.bridge.ReadableArray;
import com.facebook.react.bridge.ReadableMap;
import com.facebook.react.bridge.ReadableType;
import com.facebook.react.common.MapBuilder;
import com.facebook.react.uimanager.SimpleViewManager;
import com.facebook.react.uimanager.ThemedReactContext;

import org.json.JSONException;

import java.io.IOException;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.HashMap;
import java.util.Map;

/**
 * Created by Neo on 2017/7/24.
 */

public class VideoViewManager extends SimpleViewManager<ChannelView> {

    public static final String REACT_CLASS = "VideoView";

    private @Nullable AbstractDraweeControllerBuilder mDraweeControllerBuilder;
    private final @Nullable Object mCallerContext;
    private static Bitmap _defaultNoImage;

    public VideoViewManager() {
        mDraweeControllerBuilder = null;
        mCallerContext = null;
    }

    public VideoViewManager(
            AbstractDraweeControllerBuilder draweeControllerBuilder,
            Object callerContext) {
        mDraweeControllerBuilder = draweeControllerBuilder;
        mCallerContext = callerContext;
    }

    @Override
    public String getName() {
        return REACT_CLASS;
    }

    public AbstractDraweeControllerBuilder getDraweeControllerBuilder() {
        return mDraweeControllerBuilder;
    }

    public Object getCallerContext() {
        return mCallerContext;
    }

    private void BRGLOG()
    {
        Log.i(REACT_CLASS, Thread.currentThread().getStackTrace()[3].getMethodName());
    }

    ////////////////////////////////////////////////////////////////////////////////////

    @Override
    protected ChannelView createViewInstance(ThemedReactContext reactContext) {
        BRGLOG();
        MainApplication.setActivity(reactContext.getCurrentActivity());
        ChannelView view = new ChannelView(reactContext, getDraweeControllerBuilder(), getCallerContext());
//        ChannelView view = new ChannelView(reactContext);
        view.setDefaultNoImage(_defaultNoImage);
        return view;
    }

    @Override
    public @Nullable
    Map<String, Integer> getCommandsMap() {
        return MapBuilder.of(
                "what", 1
        );
    }

    String getNativeType(ReadableType type) {
        String tmp = "???";

        if (type == ReadableType.Null)
            tmp = "Null";
        else if (type == ReadableType.Boolean)
            tmp = "Boolean";
        else if (type == ReadableType.Number)
            tmp = "Number";
        else if (type == ReadableType.String)
            tmp = "String";
        else if (type == ReadableType.Map)
            tmp = "Map";
        else if (type == ReadableType.Array)
            tmp = "Array";

        return tmp;
    }

    public void receiveCommand(ChannelView cv, int commandId, @Nullable ReadableArray args) {
        if (args.size() >= 1) {
            String funcName = args.getString(0);
            try {
                Method method = this.getClass().getDeclaredMethod(funcName, new Class[]{ChannelView.class, ReadableArray.class});
                method.invoke(this, new Object[]{cv, args});
            } catch (NoSuchMethodException e) {
                e.printStackTrace();
            } catch (InvocationTargetException e) {
                e.printStackTrace();
            } catch (IllegalAccessException e) {
                e.printStackTrace();
            }
        }
    }

    @Override
    public @Nullable Map getExportedCustomDirectEventTypeConstants(){
        return MapBuilder.of(
                "onNativeCallback",
                MapBuilder.of("registrationName", "onNativeCallback")
        );
    }


    ////////////////////////////////////////////////////////////////////////////////////
    // Interface
    ////////////////////////////////////////////////////////////////////////////////////
    public void Start(final ChannelView cv, final ReadableArray args) {

        new Thread(new Runnable() {
            @Override
            public void run() {

                //ReadableMap mm = args.getMap(1);
                ReadableArray mm1 = args.getArray(1);
                ReadableMap mm = mm1.getMap(0);
                String ip = null;
                if (mm.hasKey(DefineTable.IP_TAG))
                    ip = mm.getString(DefineTable.IP_TAG);
                int port;
                if (ReadableType.Number == mm.getType(DefineTable.PORT_TAG))
                    port = mm.getInt(DefineTable.PORT_TAG);
                else
                    port = Integer.valueOf(mm.getString(DefineTable.PORT_TAG));
                String user = "";
                String pwd = "";
                if (mm.hasKey(DefineTable.USER_TAG)) user = mm.getString(DefineTable.USER_TAG);
                if (mm.hasKey(DefineTable.PWD_TAG)) pwd = mm.getString(DefineTable.PWD_TAG);

                int ch = 0;
                if (mm.hasKey(DefineTable.CHANNELID_TAG))
                    ch = mm.getInt(DefineTable.CHANNELID_TAG);

                String uri = mm.getString(DefineTable.URI_TAG);

                cv.Start( ip, port, user, pwd, ch, 0, uri);
            }
        }).start();
    }

    public void Stop(ChannelView cv, ReadableArray args) {
        cv.Stop();
    }
}