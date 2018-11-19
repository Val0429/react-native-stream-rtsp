package com.isap.codec;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.util.Log;

import com.isap.CodecDef;
import com.isap.RtspDelegate;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.Map;

/**
 * Created by Neo on 2017/11/22.
 */

public class RtspConnector {

    private static Map<Long, RtspDelegate> _delegateList = new HashMap<Long, RtspDelegate>();

    public void initRTSP(RtspDelegate delegate) {

        if (_delegateList.containsValue(delegate))
            return;

        Long key = System.currentTimeMillis();
        _delegateList.put(key, delegate);

        initRTSP(key);
    }

    public void deinitRTSP(RtspDelegate delegate) {
        for (Map.Entry<Long, RtspDelegate> entry : _delegateList.entrySet()) {
            if (entry.getValue() == delegate) {
                _delegateList.remove(entry.getKey());
                break;
            }
        }
        deinitRTSP();
    }

    public native void initRTSP(long key);
    public native void deinitRTSP();
    public native void start(String ip, int port, String user, String pwd, String url);
    public native void stop();

    static {
        try {
            System.loadLibrary("RtspConnector");
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public static void didReceiveVideoData(long key, int mediaType, boolean isIFrame, byte[] data, int size) {
        if (!_delegateList.containsKey(key))
            return;

        CodecDef.MediaType mType = CodecDef.MediaType.VIDEO_Nonsupport;
        switch (mediaType) {
            case 0:
                mType = CodecDef.MediaType.VIDEO_MJPEG;
                break;
            case 1:
                mType = CodecDef.MediaType.VIDEO_MPEG4;
                break;
            case 2:
                mType = CodecDef.MediaType.VIDEO_H264;
                break;
            case 3:
                mType = CodecDef.MediaType.VIDEO_H265;
                break;
        }

        RtspDelegate delegate = _delegateList.get(key);
        if (delegate != null)
            delegate.didReadVideoData(data, mType, isIFrame?CodecDef.FrameType.FRAME_TYPE_I:CodecDef.FrameType.FRAME_TYPE_P);
    }

    public static void didReceiveAudioData(long key, int sample, int bits, int channel, int mediaType, byte[] data, int size) {
        if (!_delegateList.containsKey(key))
            return;

        CodecDef.MediaType mType = CodecDef.MediaType.AUDIO_Nonsupport;
        switch (mediaType) {
            case 0:
                mType = CodecDef.MediaType.AUDIO_AAC;
                break;
        }

        RtspDelegate delegate = _delegateList.get(key);
        if (delegate != null)
            delegate.didReadAudioData(data, sample, bits, channel, mType);
    }

    public void sayHello2(byte[] data) {
        Log.e("", "Hello2");

//        if (_delegate != null)
//            _delegate.didReadData(data);
    }

    public static void sayHello3(long key, byte[] data, int size) {
        if (!_delegateList.containsKey(key))
            return;


    }
}