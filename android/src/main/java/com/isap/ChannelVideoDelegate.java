package com.isap;

import com.isap.vuexpro.model.Channel;

/**
 * Created by Neo on 2017/8/21.
 */

public interface ChannelVideoDelegate {
    void didFrameUpdated(Channel ch);
    void didPlaybackUpdateTimeRange(long start, long end);
    void didPlaybackUpdateTimeCode(long timecode);
    void didRecordFinished(String output);
}
