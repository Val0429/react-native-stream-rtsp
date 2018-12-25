package com.isap.vuexpro.Stream;


import android.util.Log;

import com.isap.ACS_VideoHeader;
import com.isap.hwdecoder.CodecDef;
import com.isap.RtspDelegate;
import com.isap.StreamSource;
import com.isap.StreamSourceDelegate;
import com.isap.codec.RtspConnector;

import java.util.LinkedList;
import java.util.Queue;
import java.util.Timer;
import java.util.TimerTask;

import static com.isap.hwdecoder.CodecDef.FrameType.FRAME_TYPE_I;
import static com.isap.hwdecoder.CodecDef.MediaType.VIDEO_Nonsupport;
import static com.isap.DefineTable.CAMERA_HIGH_RESOLUTION;
import static com.isap.DefineTable.CAMERA_LOW_RESOLUTION;

/**
 * Created by Neo on 2017/11/22.
 */

public class RTSP_StreamSource extends StreamSource implements RtspDelegate {

    private RTSP_StreamSource _context;

    private RtspConnector _rtspConnector = null;

    private final int MAX_AVAILABLE_FPS = 15;
    private Thread m_thread = null;
    private boolean m_isRunning = false;
//    private Queue<JobItem> m_queueJob = new LinkedList<JobItem>();
    private final Object m_lock = new Object();

    private final long NO_DATA_CHECK_TIME = 7 * 1000;
    private Timer m_timer;
    private long m_lastUpdateTime = 0;

    private ACS_VideoHeader _videoHeader = new ACS_VideoHeader();

    public RTSP_StreamSource(String name,
                             String model,
                             String easylinkid,
                             String ip,
                             int port,
                             String user,
                             String pwd,
                             int channelid,
                             int profileid,
                             String cgi,
                             StreamSourceDelegate delegate) {
        super(name, model, easylinkid, ip, port, user, pwd, channelid, profileid, cgi, delegate);
        _mediaType = VIDEO_Nonsupport;
        _context = this;
    }

    ////////////////////////////////////////////////////////////////////////////////////
    // Private functions
    ////////////////////////////////////////////////////////////////////////////////////


    public class retryCheck extends TimerTask
    {
        public void run()
        {
            if (System.currentTimeMillis() - m_lastUpdateTime > NO_DATA_CHECK_TIME) {
                doStop();
                doPlay();
            }
        }
    };

    private void convertNIPCAMediaType(int codingType) {
        switch (codingType) {
            case ACS_VideoHeader.VFCT_H264_IFRM:
                _videoFrameType = FRAME_TYPE_I;
                _mediaType = CodecDef.MediaType.VIDEO_H264;
                break;
            case ACS_VideoHeader.VFCT_H264_PFRM:
                _videoFrameType = CodecDef.FrameType.FRAME_TYPE_P;
                _mediaType = CodecDef.MediaType.VIDEO_H264;
                break;
            case ACS_VideoHeader.VFCT_JPEG:
                _mediaType = CodecDef.MediaType.VIDEO_MJPEG;
                break;
            default:
                break;
        }
    }

    private void doChangeAction(ACTION_STATUS status) {
        boolean available = false;

        _connectionLock.lock();
        available = _connectionStatus == CONNECTION_STATUS.CONNECTION_STATUS_DISCONNECTED;
        _connectionLock.unlock();

        if (!available)
            return;

        _actionLock.lock();
        _actionStatus = status;
        doReport();
        _actionLock.unlock();
    }

    private void connect() {
        if (_rtspConnector != null)
            return;

        _rtspConnector = new RtspConnector();
        _rtspConnector.initRTSP(_context);
        _rtspConnector.start(_ip, _port, _user, _pwd, _cmdCGI);

        m_lastUpdateTime = System.currentTimeMillis();
        m_timer = new Timer(true);
        m_timer.schedule(new retryCheck(), NO_DATA_CHECK_TIME, NO_DATA_CHECK_TIME);
    }

    private void disconnect() {
        m_timer.cancel();
        m_timer = null;
        if (_rtspConnector != null) {
            _rtspConnector.stop();
            _rtspConnector.deinitRTSP();
        }
        _rtspConnector = null;
    }


    ////////////////////////////////////////////////////////////////////////////////////
    // Public functions
    ////////////////////////////////////////////////////////////////////////////////////

    public boolean doConnect()
    {
        return true;
    }

    public void doDisconnect()
    {
        disconnect();
    }

    public void doPlay()
    {
        connect();
        doChangeAction(ACTION_STATUS.ACTION_STATUS_PLAY);
    }

    public void doPause()
    {
        doChangeAction(ACTION_STATUS.ACTION_STATUS_PAUSE);
        disconnect();
    }

    public void doStop()
    {
        doChangeAction(ACTION_STATUS.ACTION_STATUS_STOP);
        disconnect();
    }

    public void setHighResolution(boolean high_resolution)
    {
        if (_profileid == ((high_resolution)? CAMERA_HIGH_RESOLUTION : CAMERA_LOW_RESOLUTION))
            return;

        if (_rtspConnector != null) {
            doStop();
            _profileid = high_resolution? CAMERA_HIGH_RESOLUTION : CAMERA_LOW_RESOLUTION;
            if (_profileid == CAMERA_HIGH_RESOLUTION)
                _cmdCGI = "/live.sdp";
            else
                _cmdCGI = "/live2.sdp";
            doPlay();
        }
        else {
            _profileid = high_resolution? CAMERA_HIGH_RESOLUTION : CAMERA_LOW_RESOLUTION;
        }
    }

    @Override
    public void didReadVideoData(byte[] data, CodecDef.MediaType mediaType, CodecDef.FrameType frameType) {
        m_lastUpdateTime = System.currentTimeMillis();
        _delegate.didReceiveVideoData(data, mediaType, frameType, System.currentTimeMillis() * 1000);
    }

    @Override
    public void didReadAudioData(byte[] data, int sampleRate, int sampleBits, int channel, CodecDef.MediaType mediaType) {
        _delegate.didReceiveAudioData(data, sampleRate, sampleBits, channel, mediaType, System.currentTimeMillis() * 1000);
    }
}
