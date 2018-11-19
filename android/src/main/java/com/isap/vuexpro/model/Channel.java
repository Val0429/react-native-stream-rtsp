package com.isap.vuexpro.model;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;

import com.isap.codec.AACDecoder;
import com.isap.ChannelVideoDelegate;
import com.isap.CodecDef;
import com.isap.ImageUtility;
import com.isap.vuexpro.Stream.RTSP_StreamSource;
import com.isap.StreamSource;
import com.isap.StreamSourceDelegate;
import com.isap.codec.AudioPlayer;
import com.isap.codec.G711Decoder;
import com.isap.codec.G726Decoder;
import com.isap.codec.VideoDecoder;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Calendar;
import java.util.concurrent.locks.ReentrantLock;


import org.json.JSONArray;

/**
 * Created by Neo on 2017/8/21.
 */

public class Channel extends Object implements StreamSourceDelegate {

    private StreamSource _video;

    protected String _name;
    protected String _model;
    protected String _easylinkid;
    protected String _ip;
    protected int _port;
    protected String _user;
    protected String _pwd;
    protected int _channelid;
    protected int _profileid;
    protected boolean _liveMode = true;
    protected ArrayList<ChannelVideoDelegate> _viewers;
    protected StreamSourceDelegate _delegate;

    protected AudioPlayer _audioPlayer = null;
    //    protected G72xDecoder _g72xDecoder;
    protected boolean _isAACInit = false;
    protected boolean _isG726Init = false;
    protected VideoDecoder _videoDecoder = null;
//    protected AvcRecorder _avcEncoder = null;

    private Object _decoderLock = new Object();
    private Object _encoderLock = new Object();

    protected short[] _audioTempBuff = null;
    protected byte[] _audioTempBuff2 = null;
    protected short[] _audioSizeQueue = null;

    private ReentrantLock _cachedFrameLock = new ReentrantLock();
    private Bitmap _cachedFrame;
    //    private ByteBuffer _audioTempBuf;
    private CodecDef.MediaType _lastVideoMediaType;

    private boolean _playSound;
    private boolean _isRecording;

    private String _connectionDebugInfo = "-_-_-";
    private int _debugFrameCount = 0;
    private final static int GC_UPDATE_TIME = 100;


    public Channel() {}
    public Channel(String model, String name, String easylinkid, String ip, int port, String user, String pwd, int channelid, int profileid)
    {
        _model = model;
        _name = name;
        _easylinkid = easylinkid;
        _ip = ip;
        _port = port;
        _user = user;
        _pwd = pwd;
        _channelid = channelid;
        _profileid = profileid;

        _cachedFrame = null;
        _viewers = new ArrayList<ChannelVideoDelegate>();
        _delegate = null;
        _playSound = false;
        _isRecording = false;
    }


    ////////////////////////////////////////////////////////////////////////////////////
    // Private funcions
    ////////////////////////////////////////////////////////////////////////////////////

    private void fnNotifyVideoFrameUpdated()
    {
        synchronized (_viewers)
        {
            for (ChannelVideoDelegate viewer : _viewers) {
                viewer.didFrameUpdated(this);
            }
        }
    }


    ////////////////////////////////////////////////////////////////////////////////////
    // Public funcions
    ////////////////////////////////////////////////////////////////////////////////////

    public void Start(String ip, int port, String user, String pwd, int channelid, int profileid, String uri) {
        _video = new RTSP_StreamSource(
                "", "", "", ip, port, user, pwd, channelid, profileid, uri, this);
        _video.doPlay();
    }

    public void Stop() {
        _video.doStop();
    }

    public boolean isRunningLiveMode() { return _liveMode; }

    public boolean getAudioDebugInfo() { return _playSound; }

    public int getProfileDebugInfo() { return _profileid; }

    public String getConnectionDebugInfo() { return _connectionDebugInfo; }

    public int getFrameDebugInfo() { return _debugFrameCount; }

    public void setDelegate(StreamSourceDelegate delegate)
    {
        _delegate = delegate;
    }

    public void joinVideoDisplayView(ChannelVideoDelegate view)
    {
        synchronized (_viewers)
        {
            if (_viewers.contains(view))
                return;

            _viewers.add(view);
        }
    }
    public void leaveVideoDisplayView(ChannelVideoDelegate view)
    {
        synchronized (_viewers)
        {
            _viewers.remove(view);
        }
    }
    public synchronized Bitmap currentImage()
    {
        Bitmap bmp = null;
        if (_cachedFrameLock.tryLock()) {
            bmp = _cachedFrame;
            _cachedFrameLock.unlock();
        }

        return bmp;
    }

    // Live
    public boolean doConnect()
    {
        return false;
    }

    public void doDisconnect()
    {
        synchronized (_decoderLock) {
            if (_videoDecoder != null)
                _videoDecoder.destory();
            _videoDecoder = null;
        }

//        synchronized (_encoderLock) {
//            if (_avcEncoder != null)
//                _avcEncoder.stopRecording();
//            _avcEncoder = null;
//        }
    }

    public void doPlay()
    {
    }

    public void doPause()
    {
    }

    public void doStop()
    {
    }

    // Playback
    public void doPlay(Calendar startTime, Calendar endTime)
    {
    }

    public void doGoto(int jumptime)
    {

    }

    public void doSpeed(int rate)
    {

    }

    public ArrayList<Integer> getRecordedDays(int year, int month)
    {
        return null;
    }
    public ArrayList<Integer> getRecordedMins(int year, int month, int day)
    {
        return null;
    }
    public JSONArray getRecordedData(Calendar startTime, Calendar endTime, int filter)
    {
        return null;
    }

    // Control
    public void setAudio(boolean isOn)
    {
        _playSound = isOn;
    }
    public void setHighResolution(boolean high_resolution)
    {
    }
    public boolean isHighResolution() {
        return false;
    }
    public void doPT(int direction)
    {
    }
    public void doZoom(int zoom)
    {
    }
    public void gotoPreset(int position)
    {
    }
    public void doTalkie(boolean isStart)
    {
    }
//    public void startMediaRecord()
//    {
//        synchronized (_encoderLock)
//        {
//            if (_isRecording) return;
//
//            if (_avcEncoder != null) {
//                _avcEncoder.stopRecording();
//                _avcEncoder.setRecorderDelegate(null);
//                _avcEncoder = null;
//            }
//            _avcEncoder = new AvcRecorder();
//            _avcEncoder.setRecorderDelegate(this);
//            _avcEncoder.startRecording();
//
//            _isRecording = true;
//        }
//    }
//    public void stopMediaRecorder()
//    {
//        synchronized (_encoderLock)
//        {
//            if (!_isRecording) return;
//
//            if (_avcEncoder != null) {
//                _avcEncoder.stopRecording();
//                _avcEncoder.setRecorderDelegate(null);
//                _avcEncoder = null;
//            }
//
//            _isRecording = false;
//        }
//    }
    public void switchDayNightMode(int mode)
    {
    }
    public int getDayNightMode()
    {
        return 0;
    }
//    public void setBrightnessValue(int value)
//    {
//        SystemUtility.setSystemBrightnessValue(value);
//    }
//    public int getBrightnessValue()
//    {
//        return SystemUtility.getSystemBrightnessValue();
//    }

    ////////////////////////////////////////////////////////////////////////////////////
    // Delegate funcions
    ////////////////////////////////////////////////////////////////////////////////////

    @Override
    public void didStatusChanged(String name, StreamSource.ACTION_STATUS action, StreamSource.CONNECTION_STATUS connection) {

        _connectionDebugInfo = connection.toString();

        if (connection != StreamSource.CONNECTION_STATUS.CONNECTION_STATUS_CONNECTED) {
            _cachedFrameLock.lock();
            _cachedFrame = null;
            _cachedFrameLock.unlock();
            fnNotifyVideoFrameUpdated();
        }

        if (_delegate != null)
            _delegate.didStatusChanged(name, action, connection);
    }

//    @Override
//    public void didReceiveData(byte[] data, byte[] info, CodecDef.MediaType type)
//    {
//    }

    @Override
    public void didReceiveVideoData(byte[] data, CodecDef.MediaType mediaType, CodecDef.FrameType frameType, long timestamp)
    {
        synchronized (_decoderLock) {
            // no data, reset and return
            if (data == null) {
                _cachedFrameLock.lock();
                _cachedFrame = null;
                _cachedFrameLock.unlock();
                fnNotifyVideoFrameUpdated();
                if (_videoDecoder != null)
                    _videoDecoder.destory();
                _videoDecoder = null;

                return;
            }

            // if received different type then reset decoder
            if (_lastVideoMediaType != mediaType) {
                if (_videoDecoder != null)
                    _videoDecoder.destory();
                _videoDecoder = null;
            }

            // if no decoder then create a new one
            if (_videoDecoder == null && mediaType != CodecDef.MediaType.VIDEO_MJPEG) {
                _lastVideoMediaType = mediaType;
                _videoDecoder = new VideoDecoder();
            }

            // decode it by type

            Bitmap frame = null;
            try {
                if (mediaType == CodecDef.MediaType.VIDEO_MJPEG) {
                    if (_isRecording)
                        frame = BitmapFactory.decodeByteArray(data, 0, data.length);
                    else {
//                        BitmapFactory.Options options = new BitmapFactory.Options();
//                        options.inSampleSize = 2;
//                        frame = BitmapFactory.decodeByteArray(data, 0, data.length, options);
                        frame = ImageUtility.decodeSampledBitmapFromArray(data, 640, 480);
                    }
                }
                else
                    frame = _videoDecoder.decodeVideo(data, mediaType);

            } catch (java.lang.OutOfMemoryError e) {
                frame = null;
                System.gc();
            }

            if (frame != null) {
                _cachedFrameLock.lock();
//                Bitmap tmp = _cachedFrame;
                _cachedFrame = frame;
//                if (mediaType == MediaType.VIDEO_MJPEG) {
//                    if (tmp != null && !tmp.isRecycled()) {
//                        tmp.recycle();
//                        System.gc();
//                    }
//                }

//                recordVideoFrame(_cachedFrame, timestamp);

                _cachedFrameLock.unlock();
                fnNotifyVideoFrameUpdated();
                if (_debugFrameCount++ > GC_UPDATE_TIME) {
                    _debugFrameCount = 0;
//                    System.gc();
                }
            } else {
                _cachedFrameLock.lock();
                Bitmap tmp = _cachedFrame;
                _cachedFrame = null;
                _cachedFrameLock.unlock();
//                if (tmp != null && !tmp.isRecycled()) {
//                    tmp.recycle();
                tmp = null;
                System.gc();
//                }
                if (_videoDecoder != null)
                    _videoDecoder.destory();
                _videoDecoder = null;
            }
        }
    }

    @Override
    public void didReceiveAudioData(byte[] data, long rate, long bits, long channels, CodecDef.MediaType type, long timestamp)
    {
        int sampleRate = (int)rate;
        int sampleBit = (int)bits;
        int sampleChannel = (int)channels;

        if (!_playSound) {
            if (_audioPlayer != null) {
                _audioPlayer.stopAllSounds();
                _audioPlayer = null;
            }
            return;
        }
        else {
            if (_audioPlayer == null)
                _audioPlayer = new AudioPlayer();
        }

        int size = 0;
        switch (type) {
            case AUDIO_G711u:
                if (_audioTempBuff2 == null) {
                    _audioTempBuff2 = new byte[data.length*2];  // After Decode711u2, size will be data.length x 2
                }
                size = G711Decoder.Decode711u2(_audioTempBuff2, data, data.length);
                _audioPlayer.playSoundBuffer(_audioTempBuff2, size, (int)rate, 16, (int)channels);
//                recordAudioFrame(_audioTempBuff2, (int)rate, (int)bits, (int)channels, timestamp);
                break;
            case AUDIO_G711a:
                if (_audioTempBuff == null)
                {
                    _audioTempBuff = new short[data.length*2];  // After Decode711a, size will be data.length x 2
                }
                size = G711Decoder.Decode711a(_audioTempBuff, data, data.length);
                _audioPlayer.playSoundBuffer(_audioTempBuff, size, (int)rate, 16, (int)channels);
                break;
            case AUDIO_G726_16:
                if (!_isG726Init) {
                    G726Decoder.init();
                    _isG726Init = true;
                    if (_audioTempBuff == null)
                    {
                        _audioTempBuff = new short[data.length*4];
                    }
                }
                size = G726Decoder.DecodeG726_16(data, data.length, _audioTempBuff);
                _audioPlayer.playSoundBuffer(_audioTempBuff, size, (int)rate, (int)bits, (int)channels);
                break;
            case AUDIO_G726_24:
                if (!_isG726Init) {
                    G726Decoder.init();
                    _isG726Init = true;
                    if (_audioTempBuff == null)
                    {
                        _audioTempBuff = new short[data.length*4];
                    }
                }
                size = G726Decoder.DecodeG726_24(data, data.length, _audioTempBuff);
                _audioPlayer.playSoundBuffer(_audioTempBuff, size, (int)rate, (int)bits, (int)channels);
                break;
            case AUDIO_G726_32:
                if (!_isG726Init) {
                    G726Decoder.init();
                    _isG726Init = true;
                    if (_audioTempBuff == null)
                    {
                        _audioTempBuff = new short[data.length*4];
                    }
                }
                size = G726Decoder.DecodeG726_32(data, data.length, _audioTempBuff);
                _audioPlayer.playSoundBuffer(_audioTempBuff, size, (int)rate, (int)bits, (int)channels);
                break;
            case AUDIO_G726_40:
                if (!_isG726Init) {
                    G726Decoder.init();
                    _isG726Init = true;
                    if (_audioTempBuff == null)
                    {
                        _audioTempBuff = new short[data.length*4];
                    }
                }
                size = G726Decoder.DecodeG726_40(data, data.length, _audioTempBuff);
                _audioPlayer.playSoundBuffer(_audioTempBuff, size, (int)rate, (int)bits, (int)channels);
                break;
            case AUDIO_AAC:
                if (!_isAACInit) {
                    AACDecoder.init();
                    _isAACInit = true;
                    if (_audioSizeQueue == null) {
                        _audioSizeQueue = new short[16];
                    }
                    if (_audioTempBuff == null)
                    {
                        _audioTempBuff = new short[data.length*16];
                    }
                }

                size = AACDecoder.peek(data, data.length, _audioSizeQueue);
                int start=0, end=0;
                for (int i=0; i<size; i++) {
                    if (_audioSizeQueue[i] <= 0)
                        break;
                    end = start + _audioSizeQueue[i];

                    byte[] tmp = Arrays.copyOfRange(data, start, end);
                    if ( 0 == AACDecoder.decode(tmp, _audioSizeQueue[i], _audioTempBuff))
                    {
                        _audioPlayer.playSoundBuffer( _audioTempBuff, AACDecoder.getDecodedSize(), AACDecoder.getSampleRate(), AACDecoder.getSampleBit(), AACDecoder.getChannels() );
                    }
                    tmp = null;
                    start += _audioSizeQueue[i];
                }
                break;
            case AUDIO_PCM:
                if (_audioPlayer != null)
                    _audioPlayer.playSoundBuffer(data, data.length, sampleRate, sampleBit, sampleChannel);
//                recordAudioFrame(data, sampleRate, sampleBit, sampleChannel, timestamp);
                break;
            default:
                break;
        }
    }

    @Override
    public void onPlaybackUpdateTimeRange(long start, long end) {
        synchronized (_viewers)
        {
            for (ChannelVideoDelegate viewer : _viewers) {
                viewer.didPlaybackUpdateTimeRange(start, end);
            }
        }
    }

    @Override
    public void onPlaybackUpdateTimeCode(long timecode) {
        synchronized (_viewers)
        {
            for (ChannelVideoDelegate viewer : _viewers) {
                viewer.didPlaybackUpdateTimeCode(timecode);
            }
        }
    }

//    @Override
//    public void recordingFinished(String outputPath) {
//        synchronized (_viewers)
//        {
//            for (ChannelVideoDelegate viewer : _viewers) {
//                viewer.didRecordFinished(outputPath);
//            }
//        }
//    }
}
