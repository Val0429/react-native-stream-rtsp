#ifdef __cplusplus
extern "C" {
#endif


#include <jni.h>

//#include <thread>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <assert.h>

#include <android/log.h>

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libswscale/swscale.h>
#include <libavdevice/avdevice.h>

#define GET_CLASS(clazz, str, b_globlal)                                    \
    do {                                                                    \
        (clazz) = env->FindClass((str));                                    \
        if (!(clazz)) {                                                     \
            return -1;                                                      \
        }                                                                   \
        if (b_globlal) {                                                    \
            (clazz) = reinterpret_cast<jclass>(env->NewGlobalRef((clazz))); \
            if (!(clazz)) {                                                 \
                return -1;                                                  \
            }                                                               \
        }                                                                   \
    } while (0)
#define GET_ID(get, id, clazz, str, args)                 \
    do {                                                  \
        (id) = env->get((clazz), (str), (args));          \
        if (!(id)) {                                      \
            return -1;                                    \
        }                                                 \
    } while (0)

/*
    # USAGE
    GET_CLASS(clazz, "full/class/name", false);
    GET_ID(GetMethodID, method, clazz, "method", "()V");
    GET_ID(GetFieldID, field, clazz, "filed", "I");
 */

#define  LOG_TAG "RTSPConnector"
#define  LOGI(...)   __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)   __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define  LOGW(...)   __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)

#define JNIREG_CLASS "com/isap/codec/RtspConnector"

#define URL_SIZE 256

static bool g_isRunning;
static char g_rtspUrl[URL_SIZE];
static int g_videoMediaType = 0, g_audioMediaType = 0;
static pthread_mutex_t mutex;
static pthread_mutex_t dataLock;
static JavaVM *gs_jvm = NULL;
jobject gs_object = NULL;


JNIEnv* jniEnv;
jclass JavaProvider;
jobject JavaObj;
jmethodID  videoCbMethod, audioCbMethod, job_id;
jstring jstrMSG = NULL;

jlong _delegateID;

bool getConnectionStatus()
{
    bool run = false;
    pthread_mutex_lock(&mutex);
    run = g_isRunning;
    pthread_mutex_unlock(&mutex);
    return run;
}

void setConnectionStatus(bool isRun)
{
    pthread_mutex_lock(&mutex);
    g_isRunning = isRun;
    pthread_mutex_unlock(&mutex);
}

void convertVideoMediaType(AVCodecID codingType)
{
    switch(codingType) {
        case AV_CODEC_ID_MJPEG:
            g_videoMediaType = 0;
            break;
        case AV_CODEC_ID_MPEG4:
            g_videoMediaType = 1;
            break;
        case AV_CODEC_ID_H264:
            g_videoMediaType = 2;
            break;
        case AV_CODEC_ID_HEVC:
            g_videoMediaType = 3;
            break;
        default:
            break;
    }
}

void convertAudioMediaType(AVCodecID codingType)
{
    switch(codingType) {
        case AV_CODEC_ID_AAC:
            g_audioMediaType = 0;
        default:
            break;
    }
}


void* runRTSPThread(void* args)
{
    // for test
//    if (false) {
//
//        if(gs_jvm->AttachCurrentThread(&jniEnv, NULL) != JNI_OK)
//        {
//            LOGE("%s: AttachCurrentThread() failed", __FUNCTION__);
//            return NULL;
//        }
//
//
////        JavaProvider = jniEnv->FindClass("com/isap/codec/RtspConnector");
//        JavaProvider = jniEnv->GetObjectClass(gs_object);
//        if (JavaProvider == NULL) {
//            return NULL;
//        }
//
//        sayHello3 = jniEnv->GetStaticMethodID(JavaProvider, "didReceiveVideoData", "(J[B)V");
//        if (sayHello3 == NULL)
//            return NULL;
//        job_id = jniEnv->GetMethodID(JavaProvider, "<init>", "()V");
//        JavaObj = jniEnv->NewObject(JavaProvider, job_id);
//
//        jbyte* buf = new jbyte[10];
//        memset(buf, 0, 10);
//        jbyteArray jbyteArr = jniEnv->NewByteArray(10);
//        jniEnv->SetByteArrayRegion(jbyteArr, 0, 10, buf);
//        delete[] buf;
////        jniEnv->CallVoidMethod(JavaObj, sayHello3, _delegateID, jbyteArr);
//        jniEnv->CallStaticVoidMethod(JavaProvider, sayHello3, _delegateID, jbyteArr);
//
//        if (gs_jvm->DetachCurrentThread() != JNI_OK) {
//            LOGE("%s: DetachCurrentThread() failed", __FUNCTION__);
//            return NULL;
//        }
//
//        return NULL;
//    }



    LOGE("runRTSPThread: %s", g_rtspUrl);

    pthread_mutex_lock(&mutex);

    char szTimeout[16];
    int toLen = sprintf(szTimeout, "%d", 30*1000*1000);
    toLen = toLen > 15 ? 15 : toLen;
    szTimeout[toLen] = 0;

    AVDictionary* rtsp_transport_opts = NULL;
    AVDictionary* rtsp_transport_opts_udp = NULL;

    AVFormatContext* avFormatContext = avformat_alloc_context();

    const char* filename = g_rtspUrl;

    av_dict_set(&rtsp_transport_opts, "stimeout", szTimeout, 0);
    av_dict_set(&rtsp_transport_opts, "rtsp_transport", "tcp", 0);
    if (avformat_open_input(&avFormatContext, filename, NULL, &rtsp_transport_opts) != 0) {
        av_dict_set(&rtsp_transport_opts_udp, "stimeout", szTimeout, 0);
        if (avformat_open_input(&avFormatContext, filename, NULL, &rtsp_transport_opts_udp) != 0) {
            if (rtsp_transport_opts)
                av_dict_free(&rtsp_transport_opts);
            if (rtsp_transport_opts_udp)
                av_dict_free(&rtsp_transport_opts_udp);
            avformat_close_input(&avFormatContext);
            avformat_free_context(avFormatContext);

            pthread_mutex_unlock(&mutex);
            return NULL;
        }
    }

    if (avformat_find_stream_info(avFormatContext, NULL) < 0) {
        if (rtsp_transport_opts)
            av_dict_free(&rtsp_transport_opts);
        if (rtsp_transport_opts_udp)
            av_dict_free(&rtsp_transport_opts_udp);
        avformat_close_input(&avFormatContext);
        avformat_free_context(avFormatContext);

        pthread_mutex_unlock(&mutex);
        return NULL;
    }

    int video_stream_index = av_find_best_stream(avFormatContext, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    int audio_stream_index = av_find_best_stream(avFormatContext, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);

    if ((video_stream_index < 0 && audio_stream_index < 0)) {
        if (rtsp_transport_opts)
            av_dict_free(&rtsp_transport_opts);
        if (rtsp_transport_opts_udp)
            av_dict_free(&rtsp_transport_opts_udp);
        avformat_close_input(&avFormatContext);
        avformat_free_context(avFormatContext);

        pthread_mutex_unlock(&mutex);
        return NULL;
    }

    if (video_stream_index >= 0)
        convertVideoMediaType(avFormatContext->streams[video_stream_index]->codec->codec_id);

    if (audio_stream_index >= 0)
        convertAudioMediaType(avFormatContext->streams[audio_stream_index]->codec->codec_id);

    pthread_mutex_unlock(&mutex);

    setConnectionStatus(true);


    /////
    if(gs_jvm->AttachCurrentThread(&jniEnv, NULL) != JNI_OK)
    {
        LOGE("%s: AttachCurrentThread() failed", __FUNCTION__);
        return NULL;
    }

    JavaProvider = jniEnv->GetObjectClass(gs_object);
    if (JavaProvider == NULL) {
        return NULL;
    }

    videoCbMethod = jniEnv->GetStaticMethodID(JavaProvider, "didReceiveVideoData", "(JIZ[BI)V");
    if (videoCbMethod == NULL)
        return NULL;

    audioCbMethod = jniEnv->GetStaticMethodID(JavaProvider, "didReceiveAudioData", "(JIIII[BI)V");
    if (audioCbMethod == NULL)
        return NULL;
    job_id = jniEnv->GetMethodID(JavaProvider, "<init>", "()V");
    JavaObj = jniEnv->NewObject(JavaProvider, job_id);

//        jniEnv->CallVoidMethod(JavaObj, sayHello3, _delegateID, jbyteArr);

    /////
    while (getConnectionStatus()) {
        AVPacket* avp = (AVPacket*)malloc(sizeof(AVPacket));
        av_init_packet(avp);
        if (av_read_frame(avFormatContext, avp) >= 0) {
            if (avp->stream_index == video_stream_index) {

                int buff_size = avp->size;
//                jbyte* buf = new jbyte[buff_size];

                jbyteArray jbyteArr = jniEnv->NewByteArray(buff_size);
//                jniEnv->SetByteArrayRegion(jbyteArr, 0, buff_size, buf);

                jniEnv->SetByteArrayRegion(jbyteArr, 0, avp->size, (jbyte*)avp->data);
                jniEnv->CallStaticVoidMethod(JavaProvider,
                                             videoCbMethod,
                                             _delegateID,
                                             g_videoMediaType,
                                             avp->flags&AV_PKT_FLAG_KEY,
                                             jbyteArr,
                                             avp->size);
                jniEnv->DeleteLocalRef(jbyteArr);
//                delete[] buf;
            }
            else if (avp->stream_index == audio_stream_index) {

                int buff_size = avp->size;
//                jbyte* buf = new jbyte[buff_size];

                jbyteArray jbyteArr = jniEnv->NewByteArray(buff_size);
//                jniEnv->SetByteArrayRegion(jbyteArr, 0, buff_size, buf);

                jniEnv->SetByteArrayRegion(jbyteArr, 0, avp->size, (jbyte*)avp->data);
                jniEnv->CallStaticVoidMethod(JavaProvider,
                                             audioCbMethod,
                                             _delegateID,
                                             0,
                                             0,
                                             0,
                                             g_audioMediaType,
                                             jbyteArr,
                                             avp->size);
                jniEnv->DeleteLocalRef(jbyteArr);
//                delete[] buf;
            }
        }
        else {
            usleep(200);
        }
        av_free_packet(avp);
        free((void*)avp);
    }


    /////

    if (gs_jvm->DetachCurrentThread() != JNI_OK) {
        LOGE("%s: DetachCurrentThread() failed", __FUNCTION__);
        return NULL;
    }
    /////

    if (rtsp_transport_opts)
        av_dict_free(&rtsp_transport_opts);

    if (rtsp_transport_opts_udp)
        av_dict_free(&rtsp_transport_opts_udp);

    if (video_stream_index >= 0)
        avFormatContext->streams[video_stream_index]->discard = AVDISCARD_ALL;

    if (audio_stream_index >= 0)
        avFormatContext->streams[audio_stream_index]->discard = AVDISCARD_ALL;

    avformat_close_input(&avFormatContext);
    avformat_free_context(avFormatContext);

    pthread_exit(0);
    return NULL;
}

/////////////////////////////////////////////////////////////////////////////////////////

JNIEXPORT void JNICALL
initRTSP( JNIEnv* env, jobject clazz, jlong key)
{
    LOGE("initRTSP");
    g_isRunning = false;

    av_register_all();
    avdevice_register_all();
    avcodec_register_all();
    avformat_network_init();

    jniEnv = env;
    gs_object = env->NewGlobalRef(clazz);

    _delegateID = key;

}

JNIEXPORT void JNICALL
deinitRTSP( JNIEnv* env, jclass clazz )
{
    LOGE("deinitRTSP");
    avformat_network_deinit();

    memset(g_rtspUrl, 0, URL_SIZE);
    g_isRunning = false;
}

JNIEXPORT void JNICALL
start( JNIEnv* env, jclass clazz, jstring ip, int port, jstring user, jstring pwd, jstring url)
{
    LOGE("start");
    if (getConnectionStatus())
        return;

    const char *nativeIP = env->GetStringUTFChars(ip, 0);
    const char *nativeUser = env->GetStringUTFChars(user, 0);
    const char *nativePwd = env->GetStringUTFChars(pwd, 0);
    const char *nativeUrl = env->GetStringUTFChars(url, 0);

    bool token = false;
    if (nativeUser != NULL && strcmp(nativeUser, "")!=0)
        token = true;

    sprintf(g_rtspUrl, "rtsp://%s%s%s%s%s:%d%s", nativeUser, token?":":"", nativePwd, token?"@":"", nativeIP, port, nativeUrl);

    pthread_t ptRtsp;
    pthread_create(&ptRtsp, NULL, runRTSPThread, NULL);


//    LOGE("Ready to start rtsp.", g_rtspUrl);

    env->ReleaseStringUTFChars(user, nativeIP);
    env->ReleaseStringUTFChars(user, nativeUser);
    env->ReleaseStringUTFChars(user, nativePwd);
    env->ReleaseStringUTFChars(user, nativeUrl);
}

JNIEXPORT void JNICALL
stop( JNIEnv* env, jclass clazz )
{
    LOGE("stop");
    setConnectionStatus(false);
    memset(g_rtspUrl, 0, URL_SIZE);
}

static JNINativeMethod gMethods[] = {
        {	"initRTSP",     "(J)V",	    (void*)initRTSP	},
        {	"deinitRTSP",   "()V", 	    (void*)deinitRTSP	},
        {	"start",  "(Ljava/lang/String;ILjava/lang/String;Ljava/lang/String;Ljava/lang/String;)V", (void*)start	},
        {	"stop",         "()V", 	    (void*)stop	},
};

static int registerNativeMethods(JNIEnv* env, const char* className,
                                 JNINativeMethod* gMethods, int numMethods)
{
    jclass clazz;
    clazz = env->FindClass(className);
    if (clazz == NULL) {
        return JNI_FALSE;
    }
    if (env->RegisterNatives(clazz, gMethods, numMethods) < 0) {
        return JNI_FALSE;
    }

    return JNI_TRUE;
}

static int registerNatives(JNIEnv* env)
{
    if (!registerNativeMethods(env, JNIREG_CLASS, gMethods,
                               sizeof(gMethods) / sizeof(gMethods[0])))
        return JNI_FALSE;

    return JNI_TRUE;
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved)
{
    JNIEnv* env = NULL;
    jint result = -1;

    if (vm->GetEnv((void**) &env, JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }
    assert(env != NULL);

    if (!registerNatives(env)) {
        return -1;
    }

    env->GetJavaVM(&gs_jvm);

    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&dataLock, NULL);

    /* success -- return valid version number */
    result = JNI_VERSION_1_6;

    return result;
}

#ifdef __cplusplus
}
#endif
