#ifdef __cplusplus
extern "C" {
#endif

#include <jni.h>
#include "g72x.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <android/log.h>

//#ifdef _DEBUG
#define  LOG_TAG "G726Decoder"
#define  LOGI(...)   __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)   __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define  LOGW(...)   __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)
//#else
//#define  LOGI(...)
//#define  LOGE(...)
//#define  LOGW(...)
//#endif

#define JNIREG_CLASS "com/isap/codec/G726Decoder"

static g726_state g726State;
static int initFlag = 0;

JNIEXPORT void JNICALL
init( JNIEnv* env, jclass clazz )
{
}

JNIEXPORT jint JNICALL
DecodeG726_16( JNIEnv* env, jclass clazz, jbyteArray inBuffer, jint length, jshortArray pcmData )
{
	if ( initFlag == 0) {
		initFlag = 1;
		g726_init_state(&g726State);
	}

    jboolean isCopy = JNI_TRUE;
    jbyte* audioFrame = env->GetByteArrayElements(inBuffer, NULL);
    jshort* audioPacket = env->GetShortArrayElements(pcmData, &isCopy);

    jbyte* s = &audioFrame[0];
    jbyte* se = &audioFrame[length];

    unsigned char code;
    unsigned short pcm;

    int j = 0;
    while (s != se) {
    	code = *s & 0x03;
    	pcm = g726_16_decoder(code, AUDIO_ENCODING_LINEAR, &g726State);
    	((unsigned short *)audioPacket)[j++] = pcm;

    	code = (*s >> 2) & 0x03;
    	pcm = g726_16_decoder(code, AUDIO_ENCODING_LINEAR, &g726State);
    	((unsigned short *)audioPacket)[j++] = pcm;

    	code = (*s >> 4) & 0x03;
    	pcm = g726_16_decoder(code, AUDIO_ENCODING_LINEAR, &g726State);
    	((unsigned short *)audioPacket)[j++] = pcm;

    	code = *s++ >> 6;
    	pcm = g726_16_decoder(code, AUDIO_ENCODING_LINEAR, &g726State);
    	((unsigned short *)audioPacket)[j++] = pcm;
    }

    env->ReleaseShortArrayElements(pcmData, audioPacket, 0);
    env->ReleaseByteArrayElements(inBuffer, audioFrame, JNI_ABORT);

    return j;
}

JNIEXPORT jint JNICALL
DecodeG726_24( JNIEnv* env, jclass clazz, jbyteArray inBuffer, jint length, jshortArray pcmData )
{
    if ( initFlag == 0) {
        initFlag = 1;
        g726_init_state(&g726State);
    }

    jboolean isCopy = JNI_TRUE;
    jbyte* audioFrame = env->GetByteArrayElements(inBuffer, NULL);
    jshort* audioPacket = env->GetShortArrayElements(pcmData, &isCopy);

    jbyte* s = &audioFrame[0];
    jbyte* se = &audioFrame[length];

    unsigned char temp;
    unsigned char code[6];
    unsigned short pcm;

    int j = 0;

    while (s != se) {

    	// 55563444 22331112 22331112 78886677 78886677 55563444
    	for (int i=0; i<6; i++)
    		code[i] = *s++;

    	temp = (code[1]>>1) & 0x07;
    	pcm = g726_24_decoder(temp, AUDIO_ENCODING_LINEAR, &g726State);
    	((unsigned short *)audioPacket)[j++] = pcm;	//	1

    	temp = (code[1]&0x01) << 1;
    	temp |= (code[1] >> 6);
    	pcm = g726_24_decoder(temp, AUDIO_ENCODING_LINEAR, &g726State);
    	((unsigned short *)audioPacket)[j++] = pcm;	//	2

    	temp = (code[1] >> 3) & 0x06;
    	temp |= ((code[0] >> 3) & 0x01);
    	pcm = g726_24_decoder(temp, AUDIO_ENCODING_LINEAR, &g726State);
    	((unsigned short *)audioPacket)[j++] = pcm;	//	3

    	temp = code[0] & 0x07;
    	pcm = g726_24_decoder(temp, AUDIO_ENCODING_LINEAR, &g726State);
    	((unsigned short *)audioPacket)[j++] = pcm;	//	4

    	temp = code[0] >> 5;
    	pcm = g726_24_decoder(temp, AUDIO_ENCODING_LINEAR, &g726State);
    	((unsigned short *)audioPacket)[j++] = pcm;	//	5

    	temp = (code[0] >> 2) & 0x04;
    	temp |= (code[3] >> 2) & 0x03;
    	pcm = g726_24_decoder(temp, AUDIO_ENCODING_LINEAR, &g726State);
    	((unsigned short *)audioPacket)[j++] = pcm;	//	6

    	temp = (code[3] << 1) & 0x06;
    	temp |= (code[3] >> 7);
    	pcm = g726_24_decoder(temp, AUDIO_ENCODING_LINEAR, &g726State);
    	((unsigned short *)audioPacket)[j++] = pcm;	//	7

    	temp = (code[3] >> 4) & 0x07;
    	pcm = g726_24_decoder(temp, AUDIO_ENCODING_LINEAR, &g726State);
    	((unsigned short *)audioPacket)[j++] = pcm;	//	8

    	temp = (code[2]>>1) & 0x07;
    	pcm = g726_24_decoder(temp, AUDIO_ENCODING_LINEAR, &g726State);
    	((unsigned short *)audioPacket)[j++] = pcm;	//	1

    	temp = (code[2]&0x01) << 1;
    	temp |= (code[2] >> 6);
    	pcm = g726_24_decoder(temp, AUDIO_ENCODING_LINEAR, &g726State);
    	((unsigned short *)audioPacket)[j++] = pcm;	//	2

    	temp = (code[2] >> 3) & 0x06;
    	temp |= ((code[5] >> 3) & 0x01);
    	pcm = g726_24_decoder(temp, AUDIO_ENCODING_LINEAR, &g726State);
    	((unsigned short *)audioPacket)[j++] = pcm;	//	3

    	temp = code[5] & 0x07;
    	pcm = g726_24_decoder(temp, AUDIO_ENCODING_LINEAR, &g726State);
    	((unsigned short *)audioPacket)[j++] = pcm;	//	4

    	temp = code[5] >> 5;
    	pcm = g726_24_decoder(temp, AUDIO_ENCODING_LINEAR, &g726State);
    	((unsigned short *)audioPacket)[j++] = pcm;	//	5

    	temp = (code[5] >> 2) & 0x04;
    	temp |= (code[4] >> 2) & 0x03;
    	pcm = g726_24_decoder(temp, AUDIO_ENCODING_LINEAR, &g726State);
    	((unsigned short *)audioPacket)[j++] = pcm;	//	6

    	temp = (code[4] << 1) & 0x06;
    	temp |= (code[4] >> 7);
    	pcm = g726_24_decoder(temp, AUDIO_ENCODING_LINEAR, &g726State);
    	((unsigned short *)audioPacket)[j++] = pcm;	//	7

    	temp = (code[4] >> 4) & 0x07;
    	pcm = g726_24_decoder(temp, AUDIO_ENCODING_LINEAR, &g726State);
    	((unsigned short *)audioPacket)[j++] = pcm;	//	8





// 34445556 11122233 11122233 66777888 66777888 34445556
//    	for (int i=0; i<6; i++)
//    		code[i] = *s++;
//
//    	temp = (code[1]>>5) & 0x07;
//    	pcm = g726_24_decoder(temp, AUDIO_ENCODING_LINEAR, &g726State);
//    	((unsigned short *)audioPacket)[j++] = pcm;	//	1
//
//    	temp = (code[1]>>2) & 0x07;
//    	pcm = g726_24_decoder(temp, AUDIO_ENCODING_LINEAR, &g726State);
//    	((unsigned short *)audioPacket)[j++] = pcm;	//	2
//
//    	temp = ((code[1]&0x03) << 2) | (code[0] >> 7);
//    	pcm = g726_24_decoder(temp, AUDIO_ENCODING_LINEAR, &g726State);
//    	((unsigned short *)audioPacket)[j++] = pcm;	//	3
//
//    	temp = (code[0]>>4) & 0x07;
//    	pcm = g726_24_decoder(temp, AUDIO_ENCODING_LINEAR, &g726State);
//    	((unsigned short *)audioPacket)[j++] = pcm;	//	4
//
//    	temp = (code[0]>>1) & 0x07;
//    	pcm = g726_24_decoder(temp, AUDIO_ENCODING_LINEAR, &g726State);
//    	((unsigned short *)audioPacket)[j++] = pcm;	//	5
//
//    	temp = ((code[0]&0x01) << 2) | (code[3] >> 6);
//    	pcm = g726_24_decoder(temp, AUDIO_ENCODING_LINEAR, &g726State);
//    	((unsigned short *)audioPacket)[j++] = pcm;	//	6
//
//    	temp = (code[3] >> 3) & 0x07;
//    	pcm = g726_24_decoder(temp, AUDIO_ENCODING_LINEAR, &g726State);
//    	((unsigned short *)audioPacket)[j++] = pcm;	//	7
//
//    	temp = code[3] & 0x07;
//    	pcm = g726_24_decoder(temp, AUDIO_ENCODING_LINEAR, &g726State);
//    	((unsigned short *)audioPacket)[j++] = pcm;	//	8
//
//    	temp = (code[2]>>5) & 0x07;
//    	pcm = g726_24_decoder(temp, AUDIO_ENCODING_LINEAR, &g726State);
//    	((unsigned short *)audioPacket)[j++] = pcm;	//	1
//
//    	temp = (code[2]>>2) & 0x07;
//    	pcm = g726_24_decoder(temp, AUDIO_ENCODING_LINEAR, &g726State);
//    	((unsigned short *)audioPacket)[j++] = pcm;	//	2
//
//    	temp = ((code[2]&0x03) << 2) | (code[5] >> 7);
//    	pcm = g726_24_decoder(temp, AUDIO_ENCODING_LINEAR, &g726State);
//    	((unsigned short *)audioPacket)[j++] = pcm;	//	3
//
//    	temp = (code[5]>>4) & 0x07;
//    	pcm = g726_24_decoder(temp, AUDIO_ENCODING_LINEAR, &g726State);
//    	((unsigned short *)audioPacket)[j++] = pcm;	//	4
//
//    	temp = (code[5]>>1) & 0x07;
//    	pcm = g726_24_decoder(temp, AUDIO_ENCODING_LINEAR, &g726State);
//    	((unsigned short *)audioPacket)[j++] = pcm;	//	5
//
//    	temp = ((code[5]&0x01) << 2) | (code[4] >> 6);
//    	pcm = g726_24_decoder(temp, AUDIO_ENCODING_LINEAR, &g726State);
//    	((unsigned short *)audioPacket)[j++] = pcm;	//	6
//
//    	temp = (code[4] >> 3) & 0x07;
//    	pcm = g726_24_decoder(temp, AUDIO_ENCODING_LINEAR, &g726State);
//    	((unsigned short *)audioPacket)[j++] = pcm;	//	7
//
//    	temp = code[4] & 0x07;
//    	pcm = g726_24_decoder(temp, AUDIO_ENCODING_LINEAR, &g726State);
//    	((unsigned short *)audioPacket)[j++] = pcm;	//	8


//    	code = (*s >> 1) & 0x07;
//    	pcm = g726_24_decoder(code, AUDIO_ENCODING_LINEAR, &g726State);
//    	((unsigned short *)audioPacket)[j++] = pcm;
//
//    	code = (*s & 0x01) << 2;
//    	code |= (*s >> 6);
//    	pcm = g726_24_decoder(code, AUDIO_ENCODING_LINEAR, &g726State);
//    	((unsigned short *)audioPacket)[j++] = pcm;
//
//    	code = (*s++&0x30) >> 3;
//    	code |= ((*s >> 3) & 0x01);
//    	pcm = g726_24_decoder(code, AUDIO_ENCODING_LINEAR, &g726State);
//    	((unsigned short *)audioPacket)[j++] = pcm;
//
//    	code = *s & 0x07;
//    	pcm = g726_24_decoder(code, AUDIO_ENCODING_LINEAR, &g726State);
//    	((unsigned short *)audioPacket)[j++] = pcm;
//
//    	code = (*s >> 5);
//    	pcm = g726_24_decoder(code, AUDIO_ENCODING_LINEAR, &g726State);
//    	((unsigned short *)audioPacket)[j++] = pcm;
//
//    	code = (*s++ & 0x10) >> 2;
//    	code |= ((*s >> 2) & 0x03);
//    	pcm = g726_24_decoder(code, AUDIO_ENCODING_LINEAR, &g726State);
//    	((unsigned short *)audioPacket)[j++] = pcm;
//
//    	code = (*s & 0x03) << 1;
//    	code |= *s >> 7;
//    	pcm = g726_24_decoder(code, AUDIO_ENCODING_LINEAR, &g726State);
//    	((unsigned short *)audioPacket)[j++] = pcm;
//
//    	code = (*s++ >> 4) & 0x70;
//    	pcm = g726_24_decoder(code, AUDIO_ENCODING_LINEAR, &g726State);
//    	((unsigned short *)audioPacket)[j++] = pcm;



//    	code = *s & 0x07;// 00000111
//    	pcm = g726_24_decoder(code, AUDIO_ENCODING_LINEAR, &g726State);
//    	((unsigned short *)audioPacket)[j++] = pcm;
//
//    	code = (*s & 0x38) >> 3;// 00111000
//    	pcm = g726_24_decoder(code, AUDIO_ENCODING_LINEAR, &g726State);
//    	((unsigned short *)audioPacket)[j++] = pcm;
//
//    	code = (*s++&0xc0) >> 5;
//    	code |= (*s & 0x01); //11000000 + 00000001
//    	pcm = g726_24_decoder(code, AUDIO_ENCODING_LINEAR, &g726State);
//    	((unsigned short *)audioPacket)[j++] = pcm;
//
//    	code = (*s >> 1) & 0x07;// 00001110
//    	pcm = g726_24_decoder(code, AUDIO_ENCODING_LINEAR, &g726State);
//    	((unsigned short *)audioPacket)[j++] = pcm;
//
//    	code = (*s >> 4) & 0x07;// 01110000
//    	pcm = g726_24_decoder(code, AUDIO_ENCODING_LINEAR, &g726State);
//    	((unsigned short *)audioPacket)[j++] = pcm;
//
//    	code = (*s++ & 0x80) >> 5;
//    	code |= (*s & 0x03);// 10000000 + 00000011
//    	pcm = g726_24_decoder(code, AUDIO_ENCODING_LINEAR, &g726State);
//    	((unsigned short *)audioPacket)[j++] = pcm;
//
//    	code = (*s >> 2) & 0x07;
//    	pcm = g726_24_decoder(code, AUDIO_ENCODING_LINEAR, &g726State);
//    	((unsigned short *)audioPacket)[j++] = pcm;
//
//    	code = *s++ >> 5;
//    	pcm = g726_24_decoder(code, AUDIO_ENCODING_LINEAR, &g726State);
//    	((unsigned short *)audioPacket)[j++] = pcm;




//    	code = *s >> 5;
//    	pcm = g726_24_decoder(code, AUDIO_ENCODING_LINEAR, &g726State);
//    	((unsigned short *)audioPacket)[j++] = pcm;
//
//    	code = (*s >> 2) & 0x07;
//    	pcm = g726_24_decoder(code, AUDIO_ENCODING_LINEAR, &g726State);
//    	((unsigned short *)audioPacket)[j++] = pcm;
//
//    	code = (*s++ & 0x03) << 1;
//    	code |= (*s & 0x80) >> 7;
//    	pcm = g726_24_decoder(code, AUDIO_ENCODING_LINEAR, &g726State);
//    	((unsigned short *)audioPacket)[j++] = pcm;
//
//    	code = (*s & 0x70) >> 4;
//    	pcm = g726_24_decoder(code, AUDIO_ENCODING_LINEAR, &g726State);
//    	((unsigned short *)audioPacket)[j++] = pcm;
//
//    	code = (*s & 0x0e) >> 1;
//    	pcm = g726_24_decoder(code, AUDIO_ENCODING_LINEAR, &g726State);
//    	((unsigned short *)audioPacket)[j++] = pcm;
//
//    	code = (*s++ & 0x01) << 2;
//    	code |= (*s & 0xc0) >> 6;
//    	pcm = g726_24_decoder(code, AUDIO_ENCODING_LINEAR, &g726State);
//    	((unsigned short *)audioPacket)[j++] = pcm;
//
//    	code = (*s & 0x34) >> 3;
//    	pcm = g726_24_decoder(code, AUDIO_ENCODING_LINEAR, &g726State);
//    	((unsigned short *)audioPacket)[j++] = pcm;
//
//    	code = *s++ & 0x07;
//    	pcm = g726_24_decoder(code, AUDIO_ENCODING_LINEAR, &g726State);
//    	((unsigned short *)audioPacket)[j++] = pcm;
    }

    env->ReleaseShortArrayElements(pcmData, audioPacket, 0);
    env->ReleaseByteArrayElements(inBuffer, audioFrame, JNI_ABORT);

    return j;
}

JNIEXPORT jint JNICALL
DecodeG726_32( JNIEnv* env, jclass clazz, jbyteArray inBuffer, jint length, jshortArray pcmData )
{
    if ( initFlag == 0) {
        initFlag = 1;
        g726_init_state(&g726State);
    }

    jboolean isCopy = JNI_TRUE;
    jbyte* audioFrame = env->GetByteArrayElements(inBuffer, NULL);
    jshort* audioPacket = env->GetShortArrayElements(pcmData, &isCopy);

    jbyte* s = &audioFrame[0];
    jbyte* se = &audioFrame[length];

    unsigned char code;
    unsigned short pcm;

    int j = 0;
    while (s != se) {
    	code = (*s & 0x0f);
    	pcm = g726_32_decoder(code, AUDIO_ENCODING_LINEAR, &g726State);
    	((unsigned short *)audioPacket)[j++] = pcm;

    	code = *s++ >> 4;
    	pcm = g726_32_decoder(code, AUDIO_ENCODING_LINEAR, &g726State);
    	((unsigned short *)audioPacket)[j++] = pcm;
    }

    env->ReleaseShortArrayElements(pcmData, audioPacket, 0);
    env->ReleaseByteArrayElements(inBuffer, audioFrame, JNI_ABORT);

    return j;
}

JNIEXPORT jint JNICALL
DecodeG726_40( JNIEnv* env, jclass clazz, jbyteArray inBuffer, jint length, jshortArray pcmData )
{
    if ( initFlag == 0) {
        initFlag = 1;
        g726_init_state(&g726State);
    }

    jboolean isCopy = JNI_TRUE;
    jbyte* audioFrame = env->GetByteArrayElements(inBuffer, NULL);
    jshort* audioPacket = env->GetShortArrayElements(pcmData, &isCopy);

    jbyte* s = &audioFrame[0];
    jbyte* se = &audioFrame[length];

    unsigned char code;
    unsigned short pcm;

    int j = 0;

    while (s != se) {

//    	code = (*s&0x0f)<<1;
//    	code |= (*s>>7);
//    	pcm  = g726_40_decoder(code, AUDIO_ENCODING_LINEAR, &g726State);
//        ((unsigned short *)audioPacket)[j++] = pcm;		// 1
//
//        code  = (*s++ & 0x70) >> 2;
//        code |= ((*s >> 2) & 0x03);
//        pcm  = g726_40_decoder(code, AUDIO_ENCODING_LINEAR, &g726State);
//        ((unsigned short *)audioPacket)[j++] = pcm;		// 2
//
//        code  = (*s & 0x03) << 3;
//        code |= (*s >> 5);
//        pcm  = g726_40_decoder(code, AUDIO_ENCODING_LINEAR, &g726State);
//        ((unsigned short *)audioPacket)[j++] = pcm;		// 3
//
//        code  = (*s++ & 0x10);
//        code |= (*s & 0x0f);
//        pcm  = g726_40_decoder(code, AUDIO_ENCODING_LINEAR, &g726State);
//        ((unsigned short *)audioPacket)[j++] = pcm;		// 4
//
//        code  = (*s++ & 0xf0) >> 3;
//        code |= ((*s >> 3) & 0x01);
//        pcm  = g726_40_decoder(code, AUDIO_ENCODING_LINEAR, &g726State);
//        ((unsigned short *)audioPacket)[j++] = pcm;		// 5
//
//        code  = (*s & 0x07) << 2;
//        code |= (*s >> 6);
//        pcm  = g726_40_decoder(code, AUDIO_ENCODING_LINEAR, &g726State);
//        ((unsigned short *)audioPacket)[j++] = pcm;		// 6
//
//        code  = (*s++ & 0x30) >> 1;
//        code |= ((*s >> 1) & 0x07);
//        pcm = g726_40_decoder(code, AUDIO_ENCODING_LINEAR, &g726State);
//        ((unsigned short *)audioPacket)[j++] = pcm;		// 7
//
//        code  = ((*s & 0x01) << 4);
//        code |= (*s++ >> 4);
//        pcm = g726_40_decoder(code, AUDIO_ENCODING_LINEAR, &g726State);
//        ((unsigned short *)audioPacket)[j++] = pcm;		// 8


//    	code  = (*s >> 3);
//    	pcm  = g726_40_decoder(code, AUDIO_ENCODING_LINEAR, &g726State);
//        ((unsigned short *)audioPacket)[j++] = pcm;
//
//        code  = (*s++ & 0x07) << 2;
//        code |= (*s >> 6);
//        pcm  = g726_40_decoder(code, AUDIO_ENCODING_LINEAR, &g726State);
//        ((unsigned short *)audioPacket)[j++] = pcm;
//
//        code  = (*s & 0x3e) >> 1;
//        pcm  = g726_40_decoder(code, AUDIO_ENCODING_LINEAR, &g726State);
//        ((unsigned short *)audioPacket)[j++] = pcm;
//
//        code  = (*s++ & 0x01) << 4;
//        code |= (*s >> 4);
//        pcm  = g726_40_decoder(code, AUDIO_ENCODING_LINEAR, &g726State);
//        ((unsigned short *)audioPacket)[j++] = pcm;
//
//        code  = (*s++ & 0x0f) << 1;
//        code |= (*s >> 7);
//        pcm  = g726_40_decoder(code, AUDIO_ENCODING_LINEAR, &g726State);
//        ((unsigned short *)audioPacket)[j++] = pcm;
//
//        code  = (*s & 0x7c) >> 2;
//        pcm  = g726_40_decoder(code, AUDIO_ENCODING_LINEAR, &g726State);
//        ((unsigned short *)audioPacket)[j++] = pcm;
//
//        code  = (*s++ & 0x03) << 3;
//        code |= (*s >> 5);
//        pcm = g726_40_decoder(code, AUDIO_ENCODING_LINEAR, &g726State);
//        ((unsigned short *)audioPacket)[j++] = pcm;
//
//        code  = (*s++ & 0x1f);
//        pcm = g726_40_decoder(code, AUDIO_ENCODING_LINEAR, &g726State);
//        ((unsigned short *)audioPacket)[j++] = pcm;




    	code  = (*s >> 3);
    	pcm  = g726_40_decoder(code, AUDIO_ENCODING_LINEAR, &g726State);
        ((unsigned short *)audioPacket)[j++] = pcm;

        code  = (*s++ & 0x07) << 2;
        code |= (*s >> 6);
        pcm  = g726_40_decoder(code, AUDIO_ENCODING_LINEAR, &g726State);
        ((unsigned short *)audioPacket)[j++] = pcm;

        code  = (*s & 0x3e) >> 1;
        pcm  = g726_40_decoder(code, AUDIO_ENCODING_LINEAR, &g726State);
        ((unsigned short *)audioPacket)[j++] = pcm;

        code  = (*s++ & 0x01) << 4;
        code |= (*s >> 4);
        pcm  = g726_40_decoder(code, AUDIO_ENCODING_LINEAR, &g726State);
        ((unsigned short *)audioPacket)[j++] = pcm;

        code  = (*s++ & 0x0f) << 1;
        code |= (*s >> 7);
        pcm  = g726_40_decoder(code, AUDIO_ENCODING_LINEAR, &g726State);
        ((unsigned short *)audioPacket)[j++] = pcm;

        code  = (*s & 0x7c) >> 2;
        pcm  = g726_40_decoder(code, AUDIO_ENCODING_LINEAR, &g726State);
        ((unsigned short *)audioPacket)[j++] = pcm;

        code  = (*s++ & 0x03) << 3;
        code |= (*s >> 5);
        pcm = g726_40_decoder(code, AUDIO_ENCODING_LINEAR, &g726State);
        ((unsigned short *)audioPacket)[j++] = pcm;

        code  = (*s++ & 0x1f);
        pcm = g726_40_decoder(code, AUDIO_ENCODING_LINEAR, &g726State);
        ((unsigned short *)audioPacket)[j++] = pcm;
    }

    env->ReleaseShortArrayElements(pcmData, audioPacket, 0);
    env->ReleaseByteArrayElements(inBuffer, audioFrame, JNI_ABORT);

    return j;
}

static JNINativeMethod gMethods[] = {
	{	"init", "()V", (void*)init	},
	{	"DecodeG726_16", "([BI[S)I", (void*)DecodeG726_16	},
	{	"DecodeG726_24", "([BI[S)I", (void*)DecodeG726_24	},
	{	"DecodeG726_32", "([BI[S)I", (void*)DecodeG726_32	},
	{	"DecodeG726_40", "([BI[S)I", (void*)DecodeG726_40	}
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

	if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
		return -1;
	}
	assert(env != NULL);

	if (!registerNatives(env)) {
		return -1;
	}
	/* success -- return valid version number */
	result = JNI_VERSION_1_4;

	return result;
}


#ifdef __cplusplus
}
#endif
