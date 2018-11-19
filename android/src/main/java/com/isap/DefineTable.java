package com.isap;

/**
 * Created by Neo on 2017/7/28.
 */

public class DefineTable {
//    public static final String SNAPSHOT_FOLDER = FileUtils.getDiskCacheDir(MainApplication.getAppContext()) + "/snapshot/";
//    public static final String LOG_FOLDER = FileUtils.getDiskCacheDir(MainApplication.getAppContext()) + "/log/";

    // bridge tag
    public static final String NAME_TAG =       "name";
    public static final String IP_TAG =         "ip";
    public static final String PORT_TAG =       "port";
    public static final String USER_TAG =       "account";
    public static final String PWD_TAG =        "password";
    public static final String MODEL_TAG =      "model";
    public static final String CHANNELID_TAG =  "channelid";
    public static final String MAC_TAG =        "mac";
    public static final String FW_TAG =         "firmware";
    public static final String EASYLINK_TAG =   "easylinkid";
    public static final String URI_TAG =        "uri";

    // device info
    public static final int NVR_LOW_RESOLUTION =        1;
    public static final int NVR_HIGH_RESOLUTION =       0;
    public static final int CAMERA_LOW_RESOLUTION =     2;
    public static final int CAMERA_HIGH_RESOLUTION =    1;


    // STREAM SOURCE NAME
    public static final String HTTP_ACS =           "HTTP_ACS";
    public static final String HTTP_MJPG =          "HTTP_MJPG";
    public static final String HTTP_ACS_WITH_AES =  "HTTP_ACS_WITH_AES";
    public static final String RTSP =               "RTSP";


    // CGI.JSON TAG
    public static final String RESET_CGI1 =             "reset1";
    public static final String RESET_CGI2 =             "reset2";
    public static final String VIDEO_STREAMSOURCE =     "video_streamsource";
    public static final String AUDIO_IN_STREAMSOURCE =  "audio_in_streamsource";
    public static final String AUDIO_OUT_STREAMSOURCE = "audio_out_streamsource";
    public static final String VIDEO_CGI =              "video_cgi";
    public static final String AUDIO_IN_CGI =           "audio_in_cgi";
    public static final String AUDIO_OUT_CGI =          "audio_out_cgi";
    public static final String VIDEO_H264_CGI =         "video_h264_cgi";
    public static final String VIDEO_H265_CGI =         "video_h265_cgi";
    public static final String VIDEO_MPEG_CGI =         "video_mpeg_cgi";
    public static final String VIDEO_MJPG_CGI =         "video_mjpg_cgi";

    public static final String PTZ_HOME =               "ptz_home";
    public static final String PTZ_UP =                 "ptz_up";
    public static final String PTZ_DOWN =               "ptz_down";
    public static final String PTZ_LEFT =               "ptz_left";
    public static final String PTZ_RIGHT =              "ptz_right";
    public static final String PTZ_UPLEFT =             "ptz_upleft";
    public static final String PTZ_UPRIGHT =            "ptz_upright";
    public static final String PTZ_DOWNLEFT =           "ptz_downleft";
    public static final String PTZ_DOWNRIGHT =          "ptz_downright";
    public static final String PTZ_STOP =               "ptz_stop";
    public static final String ZOOM_WIDE =              "zoom_wide";
    public static final String ZOOM_TELE =              "zoom_tele";
    public static final String ZOOM_STOP =              "zoom_stop";

    public static final String  DAY_NIGHT_MODE =        "day_night_mode";
    public static final String  SPEAKER_VOLUME_CGI =    "speaker_volume_cgi";
    public static final String  PRESET_LIST_CGI =       "preset_list_cgi";
    public static final String  GOTO_PRESET_CGI =       "goto_preset_cgi";

}
