package com.isap;

//import com.isap.codec.CodecDef;
//import com.isap.socket.AsyncSocketEx;
//import com.koushikdutta.async.ByteBufferList;
//import com.vuexpro.bridge.Utility.OrbwebUtility;

import org.json.JSONArray;

import java.util.ArrayList;
import java.util.Calendar;
import java.util.concurrent.locks.ReentrantLock;

//import static com.isap.codec.CodecDef.MediaType.MEDIA_TYPE_UNUSE;


/**
 * Created by Neo on 2017/8/21.
 */

public class StreamSource extends Object {

    private final static String HEADER_AUTH = "";

    public enum ACTION_STATUS {
        ACTION_STATUS_IDLE,
        ACTION_STATUS_PLAY,
        ACTION_STATUS_PAUSE,
        ACTION_STATUS_STOP,
    }

    public enum CONNECTION_STATUS {
        CONNECTION_STATUS_IDLE,
        CONNECTION_STATUS_CONNECTING,
        CONNECTION_STATUS_CONNECTED,
        CONNECTION_STATUS_DISCONNECTED,
    }

//    protected AsyncSocketEx _sock = null;
//    protected ByteBufferList _dataBuffer = null;
    protected long _payloadSize = 0;

    protected ReentrantLock _actionLock = new ReentrantLock();
    protected ReentrantLock _connectionLock = new ReentrantLock();

    protected ACTION_STATUS _actionStatus;
    protected CONNECTION_STATUS _connectionStatus;
    protected boolean _retry;

    protected CodecDef.MediaType _mediaType;
    protected CodecDef.FrameType _videoFrameType;
    protected long _audioSampleRate;
    protected long _audioSampleBit;
    protected long _audioChannel;

    protected String _name;
    protected String _model;
    protected String _ip;
    protected int _port;
    protected String _user;
    protected String _pwd;
    protected int _channelid;
    protected int _profileid;
    protected String _cmdCGI;
    protected String _easylinkid;

    protected StreamSourceDelegate _delegate;

    protected int _tag;

    protected final static int FORCE_RETRY_CONNECTION_TIME = 1000 * 60 * 30;
    protected long _forceRetryStartTime = 0;

    protected void doReport() {
//        String action = "";
//        String connection = "";
//
//        switch(_actionStatus) {
//            case ACTION_STATUS_IDLE:
//                action = "IDLE";
//                break;
//            case ACTION_STATUS_PAUSE:
//                action = "PAUSE";
//                break;
//            case ACTION_STATUS_PLAY:
//                action = "PLAY";
//                break;
//            case ACTION_STATUS_STOP:
//                action = "STOP";
//                break;
//            default:
//                break;
//        }
//
//        switch (_connectionStatus) {
//            case CONNECTION_STATUS_IDLE:
//                connection = "IDLE";
//                break;
//            case CONNECTION_STATUS_CONNECTING:
//                connection = "CONNECTING";
//                break;
//            case CONNECTION_STATUS_CONNECTED:
//                connection = "CONNECTED";
//                break;
//            case CONNECTION_STATUS_DISCONNECTED:
//                connection = "DISCONNECTED";
//                break;
//            default:
//                break;
//        }

//        if (_delegate != null)
//            _delegate.didStatusChanged(_name, _actionStatus, _connectionStatus);
    }

    public StreamSource()
    {
    }

    public StreamSource(String name,
                        String model,
                        String easylinkid,
                        String ip,
                        int port,
                        String user,
                        String pwd,
                        int channelid,
                        int profileid,
                        String cgi,
                        StreamSourceDelegate delegate)
    {
//        _dataBuffer = new ByteBufferList();
//        _dataBuffer.setMaxItemSize(?);
//        _dataBuffer.setMaxPoolSize(?);

        _actionStatus = ACTION_STATUS.ACTION_STATUS_IDLE;
        _connectionStatus = CONNECTION_STATUS.CONNECTION_STATUS_IDLE;
        _retry = true;

        _model = model;
        _name = name;
        _ip = ip;
        _port = port;
        _user = user;
        _pwd = pwd;
        _channelid = channelid;
        _profileid = profileid;
        _easylinkid = easylinkid;

        _cmdCGI = cgi;
        _delegate = delegate;
    }

    @Override
    protected void finalize() throws Throwable {
        super.finalize();
        _retry = false;
        _delegate = null;
//        _sock = null;
    }

    public boolean doConnect()
    {
        // TODO:
        return false;
    }

    public void doDisconnect()
    {
        // TODO:
    }

    public void doPlay()
    {
        // TODO:
    }

    public void doPause()
    {
        // TODO:
    }

    public void doStop()
    {
        // TODO:
    }

    // Playback
    public void doPlay(Calendar startTime, Calendar endTime)
    {
        //TODO
    }
    public void doGoto(int jumptime)
    {
        //TODO
    }

    public void doSpeed(int rate)
    {
        //TODO
    }

    public ArrayList<Integer> getRecordedDays(int year, int month)
    {
        //TODO
        return null;
    }
    public ArrayList<Integer> getRecordedMins(int year, int month, int day)
    {
        //TODO
        return null;
    }
    public JSONArray getRecordedData(Calendar startTime, Calendar endTime, int filter)
    {
        //TODO
        return null;
    }

    public void setHighResolution(boolean high_resolution)
    {

    }
}
