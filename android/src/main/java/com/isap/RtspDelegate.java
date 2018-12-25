package com.isap;

import com.isap.hwdecoder.CodecDef;

/**
 * Created by Neo on 2017/11/23.
 */

public interface RtspDelegate {
    void didReadVideoData(byte[] data, CodecDef.MediaType mediaType, CodecDef.FrameType frameType);
    void didReadAudioData(byte[] data, int sampleRate, int sampleBits, int channel, CodecDef.MediaType mediaType);
}
