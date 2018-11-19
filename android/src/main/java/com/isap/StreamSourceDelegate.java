package com.isap;


/**
 * Created by Neo on 2017/8/21.
 */

public interface StreamSourceDelegate {
    void didStatusChanged(String name, StreamSource.ACTION_STATUS action, StreamSource.CONNECTION_STATUS connection);

    //    void didReceiveData(byte[] data, byte[] info, CodecDef.MediaType type);
    void didReceiveVideoData(byte[] data, CodecDef.MediaType mediaType, CodecDef.FrameType frameType, long timestamp);
    void didReceiveAudioData(byte[] data, long rate, long bits, long channels, CodecDef.MediaType type, long timestamp);

    void onPlaybackUpdateTimeRange(long start, long end);
    void onPlaybackUpdateTimeCode(long timecode);
}
