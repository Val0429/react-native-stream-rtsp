#ifdef __cplusplus
extern "C" {
#endif

#include <jni.h>
#include "faad.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <android/log.h>
#define  LOG_TAG "faad2_decode_aac_adts"
#define  LOGI(...)   __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)   __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define  LOGW(...)   __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)

#define JNIREG_CLASS "com/isap/codec/AACDecoder"

#define FRAME_MAX_LEN 1024*16
#define BUFFER_MAX_LEN 1024*1024

unsigned int _samplerate;
unsigned int _channels;
unsigned int _samples;

NeAACDecHandle _aac = NULL;
NeAACDecConfigurationPtr _aacConfig = NULL;
int _initFlag = 0;
int _decodedSize = 0;

JNIEXPORT void JNICALL
init(JNIEnv *env, jclass clazz) {
    _initFlag = 0;
    _aac = NeAACDecOpen();
    _aacConfig = NeAACDecGetCurrentConfiguration(_aac);
//    _aacConfig->defSampleRate = 16000;
//    _aacConfig->defObjectType = LC;
//    _aacConfig->outputFormat = FAAD_FMT_16BIT;
//    _aacConfig->downMatrix = 1;

    NeAACDecSetConfiguration(_aac, _aacConfig);
}

JNIEXPORT jint JNICALL
peek(JNIEnv *env, jclass clazz, jbyteArray buf, jint buf_len, jshortArray output) {
    int ret = 0;
    NeAACDecFrameInfo hInfo;

    jbyte *b = (jbyte *) env->GetByteArrayElements(buf, NULL);
    short *out = (short *) env->GetShortArrayElements(output, NULL);

    if (_initFlag == 0) {
        _initFlag = 1;
        unsigned long mysamplerate;
        unsigned char mychannels;

        NeAACDecInit(_aac, (unsigned char *) b, buf_len, &mysamplerate, &mychannels);

        _samplerate = mysamplerate;
        _channels = mychannels;
    }

    unsigned char *p = (unsigned char *) b;

    do {
        void *out1 = NeAACDecDecode(_aac, &hInfo, p, buf_len);
        if (hInfo.error == 0 && hInfo.samples > 0 && out1 != NULL) {
            _samples = hInfo.samples;
            p += hInfo.bytesconsumed;
            buf_len -= hInfo.bytesconsumed;
            out[ret] = hInfo.bytesconsumed;

            ret++;
        } else if (hInfo.error != 0) {
            ret = 0;
            break;
        }
    } while (buf_len > 0);

    env->ReleaseShortArrayElements(output, out, 0);
    env->ReleaseByteArrayElements(buf, b, 0);

    return ret;
}

int raw_decode(jbyte *buf, int buf_len, jshort *output) {
    int ret = 0;
    NeAACDecFrameInfo hInfo;

    if (_initFlag == 0) {
        _initFlag = 1;
        unsigned long mysamplerate;
        unsigned char mychannels;

//static int freq[] = { 8000};

        NeAACDecInit(_aac, (
                unsigned char *) buf, buf_len, &mysamplerate, &mychannels);
//NeAACDecInit2 (_aac, (unsigned char *) buf, buf_len, (void*)&freq, &mychannels);
        _samplerate = mysamplerate;
        _channels = mychannels;
    }

//short buf1[ 1024 * 4 ] = {0};
    unsigned char *p = (unsigned char *) buf;

    do {
        void *out1 = NeAACDecDecode(_aac, &hInfo, p, buf_len);
        if ((hInfo.error == 0) && (hInfo.samples > 0)) {
            _samples = hInfo.samples;
            p += hInfo.
                    bytesconsumed;
            buf_len -= hInfo.
                    bytesconsumed;

// distill wave
            short *p1 = output, *p2 = (short *) out1;
            for (
                    int k = (hInfo.samples / hInfo.channels);
                    k;
                    k--) {
                *p1++ = *
                        p2;
                p2 += 2;
                ret += 2;
            }

//ret = hInfo.samples;
        } else if (hInfo.error != 0) {
            ret = 0;
            break;
        }
    } while (buf_len > 0);
    return
            ret;
}

JNIEXPORT jint JNICALL
decode(JNIEnv *env, jclass clazz, jbyteArray buf, jint buf_len, jshortArray output) {

//    jbyte *src = (jbyte *) env->GetByteArrayElements(buf, NULL);
//    jshort *dest = (jshort *) env->GetShortArrayElements(output, NULL);
//    int ret = raw_decode(src, buf_len, dest);
//    env->ReleaseShortArrayElements(output, dest, 0);
//    env->ReleaseByteArrayElements(buf, src, 0);
//
//    return ret;

    int ret = 0;
    NeAACDecFrameInfo hInfo;

    jbyte *b = (jbyte *) env->GetByteArrayElements(buf, NULL);
    jshort *p1 = (jshort *) env->GetShortArrayElements(output, NULL);

    if (_initFlag == 0) {
        _initFlag = 1;
        unsigned long mysamplerate;
        unsigned char mychannels;

        NeAACDecInit(_aac, (unsigned char *) b, buf_len, &mysamplerate, &mychannels);

        _samplerate = mysamplerate;
        _channels = mychannels;
    }

    unsigned char *p = (unsigned char *) b;
    _decodedSize = 0;

    do {
        void *out1 = NeAACDecDecode(_aac, &hInfo, p, buf_len);
        if (hInfo.error == 0 && hInfo.samples > 0 && out1 != NULL) {
            _samples = hInfo.samples;
            p += hInfo.bytesconsumed;
            buf_len -= hInfo.bytesconsumed;


            jshort *p2 = (jshort *) out1;
            jshort *temp_pi = p1;
            for (int k = (hInfo.samples / hInfo.channels); k; k--) {
                *temp_pi++ = *p2;
                p2 += 2;
                _decodedSize += 1;
            }

        } else if (hInfo.error != 0) {
            ret = -1;
            break;
        }
    } while (buf_len > 0);

    env->ReleaseShortArrayElements(output, p1, 0);
    env->ReleaseByteArrayElements(buf, b, 0);

    return ret;
}

jbyteArray
shortToByte(JNIEnv *env, jobject thiz, jshortArray input) {
    jshort *input_array_elements;
    int input_length;

    jbyte *output_array_elements;
    jbyteArray output;

    input_array_elements = env->GetShortArrayElements(input, 0);
    input_length = env->GetArrayLength(input);

    output = (jbyteArray) (env->NewByteArray(input_length * 2));
    output_array_elements = env->GetByteArrayElements(output, 0);

    memcpy(output_array_elements, input_array_elements, input_length * 2);

    env->ReleaseShortArrayElements(input, input_array_elements, JNI_ABORT);
    env->ReleaseByteArrayElements(output, output_array_elements, 0);

    return output;
}

JNIEXPORT jint JNICALL
getSampleRate(JNIEnv *env, jobject thiz) {
    return _samplerate;
}

JNIEXPORT jint JNICALL
getChannels(JNIEnv *env, jobject thiz) {
    return _channels;
}

JNIEXPORT jint JNICALL
getSampleBit(JNIEnv *env, jobject thiz) {
    return _samples;
}

JNIEXPORT jstring JNICALL native_hello(JNIEnv *env, jclass clazz) {
    printf("hello in c native code./n");
    return env->NewStringUTF("hello world returned.");
}

/**
 * fetch one ADTS frame
 */
int get_one_ADTS_frame(unsigned char *buffer, size_t buf_size,
                       unsigned char *data, size_t *data_size) {
    size_t size = 0;

    if (!buffer || !data || !data_size) {
        return -1;
    }

    while (1) {
        if (buf_size < 7) {
            return -1;
        }

        if ((buffer[0] == 0xff) && ((buffer[1] & 0xf0) == 0xf0)) {
            size |= ((buffer[3] & 0x03) << 11); //high 2 bit
            size |= buffer[4] << 3;//middle 8 bit
            size |= ((buffer[5] & 0xe0) >> 5);//low 3bit
            break;
        }
        --buf_size;
        ++buffer;
    }

    if (buf_size < size) {
        return -1;
    }

    memcpy(data, buffer, size);
    *data_size = size;

    return 0;
}

JNIEXPORT jint JNICALL
decodeAAC(JNIEnv *env, jclass clazz, jbyteArray buf, jint buf_len, jbyteArray output) {

    static unsigned char frame[FRAME_MAX_LEN];

    char src_file[128] = {0};
    char dst_file[128] = {0};

    unsigned long samplerate;
    unsigned char channels;
    NeAACDecHandle decoder = 0;

    size_t data_size = 0;
    size_t size = 0;

    jbyte *bIn = env->GetByteArrayElements(buf, NULL);
    unsigned char *buffer = (unsigned char *) bIn;

    jbyte *bOut = env->GetByteArrayElements(output, NULL);
    unsigned char *ucOut = (unsigned char *) bOut;

    NeAACDecFrameInfo frame_info;
    unsigned char *input_data = buffer;
    unsigned char *pcm_data = NULL;

    //open decoder
    decoder = NeAACDecOpen();
    if (get_one_ADTS_frame(buffer, buf_len, frame, &size) < 0) {
        return -1;
    }

    //initialize decoder
    NeAACDecInit(decoder, frame, size, &samplerate, &channels);
    LOGI("samplerate %ld, channels %d\n", samplerate, channels);

    while (get_one_ADTS_frame(input_data, buf_len, frame, &size) == 0) {
        //decode ADTS frame
        pcm_data = (unsigned char *) NeAACDecDecode(decoder, &frame_info, frame,
                                                    size);

        if (frame_info.error > 0) {
            LOGE("error: %s\n", NeAACDecGetErrorMessage(frame_info.error));
            LOGE(
                    "frame info: bytesconsumed %ld, channels %d, header_type %d\
	                object_type %d, samples %ld, samplerate %ld\n",
                    frame_info.bytesconsumed, frame_info.channels,
                    frame_info.header_type, frame_info.object_type,
                    frame_info.samples, frame_info.samplerate);
        } else if (pcm_data && frame_info.samples > 0) {
            LOGE(
                    "frame info: bytesconsumed %ld, channels %d, header_type %d\
	                object_type %d, samples %ld, samplerate %ld\n",
                    frame_info.bytesconsumed, frame_info.channels,
                    frame_info.header_type, frame_info.object_type,
                    frame_info.samples, frame_info.samplerate);

            memcpy(ucOut, pcm_data, frame_info.samples * frame_info.channels);
            ucOut += frame_info.samples * frame_info.channels;
        }
        buf_len -= size;
        input_data += size;
    }

    NeAACDecClose(decoder);

    return 0;
}

JNIEXPORT jint JNICALL
getDecodedSize(JNIEnv *env, jclass clazz) {
    return _decodedSize;
}

static JNINativeMethod gMethods[] = {
        {"init",           "()V",                  (void *) init},
        {"peek",           "([BI[S)I",             (void *) peek},
        {"decode",         "([BI[S)I",             (void *) decode},
        {"hello",          "()Ljava/lang/String;", (void *) native_hello},
        {"shortToByte",    "([S)[B",               (void *) shortToByte},
        {"decodeAAC",      "([BI[B)I",             (void *) decodeAAC},
        {"getDecodedSize", "()I",                  (void *) getDecodedSize},
        {"getSampleRate",  "()I",                  (void *) getSampleRate},
        {"getChannels",    "()I",                  (void *) getChannels},
        {"getSampleBit",   "()I",                  (void *) getSampleBit}
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
    /* success -- return valid version number */
    result = JNI_VERSION_1_6;

    return result;
}

#ifdef __cplusplus
}
#endif
