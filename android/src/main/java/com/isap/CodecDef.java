package com.isap;

/**
 * Created by Neo on 2017/8/21.
 */

public final class CodecDef {


    public enum MediaType {
        MEDIA_TYPE_UNUSE,
        AUDIO_Nonsupport,
        AUDIO_PCM,
        AUDIO_AAC,
        AUDIO_G711u,
        AUDIO_G711a,
        AUDIO_G726_16,
        AUDIO_G726_24,
        AUDIO_G726_32,
        AUDIO_G726_40,

        VIDEO_Nonsupport,
        VIDEO_H264,
        VIDEO_H265,
        VIDEO_MPEG4,
        VIDEO_MJPEG,
        MEDIA_TYPE_MAX
    }

    public enum FrameType {
        FRAME_TYPE_UNKNOW,
        FRAME_TYPE_I,
        FRAME_TYPE_P,
        FRAME_TYPE_B,
        FRAME_TYPE_MAX
    }
}
