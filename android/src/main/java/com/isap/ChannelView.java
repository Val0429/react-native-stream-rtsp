package com.isap;


import android.content.Context;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Matrix;
import android.graphics.Paint;
import android.graphics.Rect;
import android.net.Uri;
import android.os.Handler;
import android.os.Message;
import android.support.annotation.Nullable;
import android.util.Log;

import com.facebook.drawee.controller.AbstractDraweeControllerBuilder;
import com.facebook.drawee.generic.GenericDraweeHierarchy;
import com.facebook.drawee.generic.GenericDraweeHierarchyBuilder;
import com.facebook.drawee.generic.RoundingParams;
import com.facebook.drawee.view.GenericDraweeView;
import com.facebook.react.bridge.Arguments;
import com.facebook.react.bridge.ReactContext;
import com.facebook.react.bridge.WritableMap;
import com.facebook.react.uimanager.events.RCTEventEmitter;
import com.isap.vuexpro.model.Channel;

import android.view.GestureDetector;
import android.view.MotionEvent;
import android.view.ScaleGestureDetector;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.locks.ReentrantLock;


/**
 * Created by Neo on 2017/7/24.
 */

public class ChannelView extends GenericDraweeView implements ChannelVideoDelegate {

    private ChannelView _handler;
    private int _bindId = -1;
    private Channel _channel;
    private Bitmap _defaultNoImage;
    private ReentrantLock _bmpLock = new ReentrantLock();
    private Bitmap _bmp = null;

    private float _actualWidth = -1.0f;
    private float _actualHeight = -1.0f;
    private float _orgWidth = -1.0f;
    private float _orgHeight = -1.0f;
    private float _scaleX = 1.0f;
    private float _scaleY = 1.0f;
    private float _curScale = 1.0f;

    private boolean _stretch = false;
    private int _rotation = 0;

    private ScaleGestureDetector _scaleGesture;
    private GestureDetector _gesture;
    private Matrix _default = new Matrix();
    private Matrix _transform = new Matrix();

    private float PADDING = 5.0f;

    private ExecutorService _singleExecutor = Executors.newSingleThreadExecutor();

    ////////////////////////////////////////////////////////////////////////////////////
    // Scale funcions
    ////////////////////////////////////////////////////////////////////////////////////

    private void updateMatrix(){
        boolean before = this.getIsZooming();

        float[] values = {0,0,0,0,0,0,0,0,0};
        _transform.getValues(values);
        float scaleX = values[0];
        _curScale = scaleX;
        if(scaleX < 1) {
            _transform.reset();
            _curScale = 1;
        }
        Matrix m = new Matrix();
        m.reset();
        m.postConcat(_default);
        m.postConcat(_transform);
        this.setImageMatrix(m);

        if(before != this.getIsZooming())
            this.OnZoomStateChanged();
    }

    private void checkMatrix(float bmWidth,float bmHeight){
        if(	bmWidth * bmHeight == 0 ||
                this.getWidth() * this.getHeight() == 0 ||
                (_orgWidth == bmWidth && _orgHeight == bmHeight))
            return;

        _orgWidth = bmWidth;
        _orgHeight = bmHeight;
        _actualWidth = this.getWidth() - PADDING*2;
        _actualHeight= this.getHeight() - PADDING*2;
        setPadding((int)PADDING, (int)PADDING, (int)PADDING, (int)PADDING);

        _default.reset();
        _default.postTranslate(-_orgWidth/2,-_orgHeight/2);

        if (_stretch)
        {
            _scaleX = _actualWidth / _orgWidth;
            _scaleY = _actualHeight / _orgHeight;
        }
        else
        {
            _scaleX = _scaleY = Math.min(_actualWidth / _orgWidth, _actualHeight / _orgHeight);
        }

        _default.postScale(_scaleX, _scaleY);

        _default.postTranslate(_actualWidth/2, _actualHeight/2);
        updateMatrix();
    }

    //
    // Double Tap Delegate
    //
    private OnDoubleTapListener dblclistener;
    public interface OnDoubleTapListener {
        void onDoubleTap(ChannelView view);
    }
    public void setOnDoubleTapListener(OnDoubleTapListener l){
        dblclistener = l;
    }
    protected void OnDoubleTap(){
        if (dblclistener != null)
            dblclistener.onDoubleTap(this);
    }

    //
    // Zooming Delegate
    //
    private OnZoomingListener zoomListener;
    public interface OnZoomingListener {
        void OnZoomStateChanged(ChannelView view);
    }
    public boolean getIsZooming(){
        return _curScale > 1;
    }
    public float getZoomScale(){
        return _curScale;
    }
    public void setOnZoomingListener(OnZoomingListener l) {
        zoomListener = l;
    }
    protected void OnZoomStateChanged(){
        if(zoomListener==null)return;
        zoomListener.OnZoomStateChanged(this);
    }

    @Override
    public void onLayout(boolean changed, int left, int top, int right, int bottom){
        super.onLayout(changed, left, top, right, bottom);
        _orgWidth = -1.0f;
        _orgHeight = -1.0f;
        _UIUpdateHandler.sendEmptyMessage(0);
    }

    @Override
    public void setImageBitmap(Bitmap bm) {
        if(bm==null || bm.isRecycled())return;
        int bmWidth = bm.getWidth();
        int bmHeight = bm.getHeight();
        checkMatrix(bmWidth,bmHeight);
        super.setImageBitmap(bm);
    }

    private int maxPoints = 0;
    private boolean isPressed = false;

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        switch(event.getActionMasked()) {
            case MotionEvent.ACTION_DOWN:
                maxPoints = 0;
                isPressed = true;
                super.onTouchEvent(event);
                break;
            case MotionEvent.ACTION_UP:
                isPressed = false;
                if(maxPoints == 1 && !this.getIsZooming()) {
                    super.onTouchEvent(event);
                }
                break;
        }

        if(isPressed) {
            maxPoints = Math.max(maxPoints, event.getPointerCount());
        }

        this._scaleGesture.onTouchEvent(event);
        this._gesture.onTouchEvent(event);
        ChannelView.this.invalidate();

        return true;
    }

    @Override
    protected void onAttachedToWindow() {
        super.onAttachedToWindow();
    }

    @Override
    protected void onDetachedFromWindow() {
//        _singleExecutor.shutdown();
        _singleExecutor = null;
//        if (_channel != null) {
//            _channel.doDisconnect();
//        }
        _channel = null;
        super.onDetachedFromWindow();
    }

    /////////////////////////////////////////////////////////////////////////////////////

    private static GenericDraweeHierarchy buildHierarchy(Context context)
    {
        return new GenericDraweeHierarchyBuilder(context.getResources())
                .setRoundingParams(RoundingParams.fromCornersRadius(0))
                .build();
    }

    public ChannelView(
            Context context,
            AbstractDraweeControllerBuilder draweeControllerBuilder,
            @Nullable Object callerContext)
    {
        super(context, buildHierarchy(context));
        _handler = this;

        this.setScaleType(ScaleType.MATRIX);
        _scaleGesture = new ScaleGestureDetector(context, new ScaleGestureDetector.OnScaleGestureListener(){
            public boolean onScale (ScaleGestureDetector detector){
                Matrix m = ChannelView.this._transform;
                m.postTranslate(-detector.getFocusX(), -detector.getFocusY());
                m.postScale(detector.getScaleFactor(), detector.getScaleFactor());
                m.postTranslate(detector.getFocusX(), detector.getFocusY());
                ChannelView.this.updateMatrix();
                return true;
            }
            public boolean onScaleBegin (ScaleGestureDetector detector){
                return true;
            }
            public void onScaleEnd (ScaleGestureDetector detector){}
        });
        _gesture = new GestureDetector(context, new GestureDetector.SimpleOnGestureListener(){
            @Override
            public boolean onScroll (MotionEvent e1, MotionEvent e2, float distanceX, float distanceY){
                if(_curScale < 1.01) return false;
                Matrix m = ChannelView.this._transform;
                m.postTranslate(-distanceX, -distanceY);
                ChannelView.this.updateMatrix();
                return true;
            }
            public boolean onDoubleTap (MotionEvent e){
                ChannelView.this.OnDoubleTap();
                return false;
            }
        });
    }

    public void setDefaultNoImage(Bitmap bmp) {
        _defaultNoImage = bmp;
    }

    public void sentNativeEvent(WritableMap event, String caller)
    {
        event.putString("caller", caller);
        ReactContext reactContext = (ReactContext)getContext();
        reactContext.getJSModule(RCTEventEmitter.class).receiveEvent(
                getId(),
                "onNativeCallback",
                event);
    }

    Bitmap newBitmap = null;
    Canvas newCanvas = null;
    Paint paintText = null;
    Rect srcRect = new Rect();
    Rect destRect = new Rect();

    private void NormalUpdate(Message msg) {
        try {
            if (!_bmpLock.tryLock())
                return;

            if (_channel != null) {
                _bmp = _channel.currentImage();
            }

            if (_bmp == null || _bmp.isRecycled())
                _handler.setImageBitmap(_defaultNoImage);
            else {
//            Bitmap tmp = _bmp.copy(Bitmap.Config.ARGB_8888, false);
                _handler.setImageBitmap(_bmp);
//            tmp = null;
            }

            _bmpLock.unlock();
        } catch (Exception e) {
            _handler.setImageBitmap(null);
            if (_bmpLock.isLocked())
                _bmpLock.unlock();
        }
    }

    Handler _UIUpdateHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
//            if (MainApplication.isIsDebugMode())
//                DebugUpdate(msg);
//            else
                NormalUpdate(msg);
        }
    };

    void drawMultiLineText(String str, float x, float y, Paint paint, Canvas canvas) {
        String[] lines = str.split("\n");
        float txtSize = -paint.ascent() + paint.descent();

        if (paint.getStyle() == Paint.Style.FILL_AND_STROKE || paint.getStyle() == Paint.Style.STROKE){
            txtSize += paint.getStrokeWidth(); //add stroke width to the text size
        }
        float lineSpace = txtSize * 0.2f;  //default line spacing

        for (int i = 0; i < lines.length; ++i) {
            canvas.drawText(lines[i], x, y + (txtSize + lineSpace) * i, paint);
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////
    // Streaming functions
    ////////////////////////////////////////////////////////////////////////////////////

    public void Start(String ip, int port, String user, String pwd, int channelid, int profileid, String uri)
    {
        if (_channel == null)
            _channel = new Channel("", "", "", ip, port, user, pwd, channelid, profileid);
        _channel.joinVideoDisplayView(this);
        _channel.Start(ip, port, user, pwd, channelid, profileid, uri);
    }

    public void Stop()
    {
        if (_channel != null) {
            _channel.leaveVideoDisplayView(this);
            _channel.Stop();
            _channel = null;
        }
    }


    ////////////////////////////////////////////////////////////////////////////////////
    // Delegate funcions
    ////////////////////////////////////////////////////////////////////////////////////

    @Override
    public void didFrameUpdated(Channel ch) {

        _UIUpdateHandler.sendEmptyMessage(0);

//        this.post(new Runnable() {
//            @Override
//            public void run() {
//                if (MainApplication.isIsDebugMode())
//                    DebugUpdate(null);
//                else
//                    NormalUpdate(null);
//            }
//        });
    }

    @Override
    public void didPlaybackUpdateTimeRange(long start, long end) {

        Log.e("", "Playback: " + start + " ~ " + end);
        WritableMap event = Arguments.createMap();
        event.putString("start", ""+start);
        event.putString("end", ""+end);
        sentNativeEvent(event, "PlaybackRange");
    }

    @Override
    public void didPlaybackUpdateTimeCode(long timecode) {
        Log.e("", "Playback pos: " + timecode);
        WritableMap event = Arguments.createMap();
        event.putString("pos", ""+timecode);
        sentNativeEvent(event, "PlaybackCurrentPosition");
    }

    @Override
    public void didRecordFinished(String output) {


//        Intent intentBc = new Intent(Intent.ACTION_MEDIA_SCANNER_SCAN_FILE);
//        intentBc.setData(Uri.parse("file://" + output));
//        _handler.getContext().sendBroadcast(intentBc);
//
//
        _handler.getContext().sendBroadcast(new Intent(Intent.ACTION_MEDIA_SCANNER_SCAN_FILE, Uri.parse("file://" + output)));
    }

//    public void scanFileAsync(Context ctx, String filePath) {
//        Intent scanIntent = new Intent(Intent.ACTION_MEDIA_SCANNER_SCAN_FILE);
//        scanIntent.setData(Uri.fromFile(new File(filePath)));
//        ctx.sendBroadcast(scanIntent);
//    }
}
