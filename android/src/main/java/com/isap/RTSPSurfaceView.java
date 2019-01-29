package com.isap;

import android.view.SurfaceView;
import android.widget.FrameLayout;

import com.facebook.react.uimanager.ThemedReactContext;
import com.isap.vuexpro.model.Channel;



public class RTSPSurfaceView extends FrameLayout implements ChannelVideoDelegate {

    RTSPSurfaceView mContext;
    boolean mIsRunning = false;

    private SurfaceView mSurfaceView;

    private Channel _channel;

    @Override
    protected void onAttachedToWindow() {
        super.onAttachedToWindow();
    }

    @Override
    protected void onDetachedFromWindow() {
        super.onDetachedFromWindow();
    }

    public void Start(String ip, int port, String user, String pwd, int channelid, int profileid, String uri)
    {
        if (_channel == null)
            _channel = new Channel("", "", "", ip, port, user, pwd, channelid, profileid, mSurfaceView);
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

    public RTSPSurfaceView(ThemedReactContext context)
    {
        super(context);

        mContext = this;

        LayoutParams layoutParams = new LayoutParams(
                LayoutParams.MATCH_PARENT,
                LayoutParams.MATCH_PARENT);

        mSurfaceView = new SurfaceView(context);
        mSurfaceView.setLayoutParams(layoutParams);
        addView(mSurfaceView, 0, layoutParams);
    }

    @Override
    public void didFrameUpdated(Channel ch) {

    }

    @Override
    public void didPlaybackUpdateTimeRange(long start, long end) {

    }

    @Override
    public void didPlaybackUpdateTimeCode(long timecode) {

    }

    @Override
    public void didRecordFinished(String output) {

    }
}
