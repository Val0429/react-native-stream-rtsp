#ifdef __cplusplus
extern "C" {
#endif

#include <jni.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <android/log.h>
#define  LOG_TAG "g711_decode_adts"
#define  LOGI(...)   __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)   __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define  LOGW(...)   __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)

#define JNIREG_CLASS "com/isap/codec/G711Decoder"

jbyte ALawEncode(jshort pcm16)
{
	int p = pcm16;
	unsigned a;
	if (p < 0) {
		p = ~p;
		a = 0x00;
	}
	else {
		a = 0x80;
	}

	p >>= 4;
	if (p >= 0x20) {
		if (p >= 0x100) {
			p >>= 4;
			a += 0x40;
		}
		if (p >= 0x40) {
			p >>= 2;
			a += 0x20;
		}
		if (p >= 0x20) {
			p >>= 1;
			a += 0x10;
		}
	}

	a += p;

	return a ^ 0x5;
}

jshort ALawDecode(jbyte alaw)
{
	alaw ^= 0x55;

	jbyte sign = alaw & 0x80;
	jshort linear = alaw & 0x1f;
	linear <<= 4;
	linear += 8;

	alaw &= 0x7f;
	if (alaw >= 0x20) {
		linear != 0x100;
		jbyte shift = (alaw>>4) - 1;
		linear <<= shift;
	}

	if (!sign)
		return -linear;
	else
		return linear;
}

jbyte ULawEncode(jshort pcm16)
{
	int p = pcm16;
	unsigned u;

	if(p < 0) {
		p = ~p;
		u = 0x80^0x10^0xff;
	}
	else {
		u = 0x00^0x10^0xff;
	}

	p += 0x84;

	if(p>0x7f00)
		p = 0x7f00;


	p >>= 3;
	if(p >= 0x100) {
		p >>= 4;
		u ^= 0x40;
	}
	if(p >= 0x40) {
		p >>= 2;
		u ^= 0x20;
	}
	if(p >= 0x20) {
		p >>= 1;
		u ^= 0x10;
	}

	u ^= p;

	return u;
}

jshort ULawDecode(jbyte ulaw)
{
	ulaw ^= 0xff;

	jshort linear = ulaw&0x0f;
	linear <<= 3;
	linear |= 0x84;

	jbyte shift = ulaw >> 4;
	shift &= 7;
	linear <<= shift;

	linear -= 0x84;

	if (ulaw & 0x80)
		return -linear;
	else
		return linear;
}

JNIEXPORT jshort JNICALL
Decode711a(JNIEnv *env, jobject thiz, jshortArray dst, jbyteArray src, jint srcSize)
{
	jshort *dst_array_elements;
	jbyte *src_array_elements;

	jshort *dst_array_elements_tmp;
	jbyte *src_array_elements_tmp;

	dst_array_elements = env->GetShortArrayElements(dst, 0);
	src_array_elements = env->GetByteArrayElements(src, 0);

	dst_array_elements_tmp = dst_array_elements;
	src_array_elements_tmp = src_array_elements;

	short* end = dst_array_elements_tmp + srcSize;
	while(dst_array_elements_tmp < end)
		*dst_array_elements_tmp++ = ALawDecode(*src_array_elements_tmp++);

	env->ReleaseShortArrayElements(dst, dst_array_elements, JNI_ABORT);
	env->ReleaseByteArrayElements(src, src_array_elements, 0);

	return srcSize << 1;
}

JNIEXPORT jshort JNICALL
Decode711u(JNIEnv *env, jobject thiz, jshortArray dst, jbyteArray src, jint srcSize)
{
	jshort *dst_array_elements;
	jbyte *src_array_elements;

	jshort *dst_array_elements_temp;
	jbyte *src_array_elements_temp;

	dst_array_elements = env->GetShortArrayElements(dst, 0);
	src_array_elements = env->GetByteArrayElements(src, 0);

	dst_array_elements_temp = dst_array_elements;
	src_array_elements_temp = src_array_elements;

	short* end = dst_array_elements_temp + srcSize;
	while(dst_array_elements_temp < end)
		*dst_array_elements_temp++ = ULawDecode(*src_array_elements_temp++);

	env->ReleaseShortArrayElements(dst, dst_array_elements, JNI_ABORT);
	env->ReleaseByteArrayElements(src, src_array_elements, 0);

	return srcSize << 1;
}

JNIEXPORT jshort JNICALL
Decode711u2(JNIEnv *env, jobject thiz, jbyteArray dst, jbyteArray src, jint srcSize)
{
	jbyte *src_array_elements = (jbyte *)env->GetByteArrayElements(src, NULL);
	jbyte *dst_array_elements = (jbyte *)env->GetByteArrayElements(dst, NULL);

	jbyte *src_array_elements_tmp = src_array_elements;
	jbyte *dst_array_elements_tmp = dst_array_elements;

	jbyte* end = dst_array_elements + srcSize*2;
	short temp;
	while(dst_array_elements_tmp < end) {
		temp = ULawDecode(*src_array_elements_tmp++);
		*dst_array_elements_tmp++ = temp&0x00ff;
		*dst_array_elements_tmp++ = (temp&0xff00)>>8;
	}

	env->ReleaseByteArrayElements( dst, dst_array_elements, 0 );
	env->ReleaseByteArrayElements( src, src_array_elements, 0 );
	return srcSize << 1;
}

//JNIEXPORT jshort JNICALL
//Encode711a(JNIEnv *env, jobject thiz, jbyteArray dst, jshortArray src, jint srcSize)
//{
//	srcSize >>= 1;
//
//    jshort *src_array_elements = (jshort *)env->GetShortArrayElements(src, NULL);
//	jbyte *dst_array_elements = (jbyte *)env->GetByteArrayElements(dst, NULL);
//
//    jbyte* end = dst_array_elements + srcSize;
//    jshort temp;
//    jshort* pSrc = src_array_elements;
//    jbyte* pDst = dst_array_elements;
//
//	while (dst_array_elements < end) {
//        temp = *pSrc++;
//        *pDst++ = ALawEncode(temp);
//	}
//
//	env->ReleaseByteArrayElements( dst, dst_array_elements, 0 );
//	env->ReleaseShortArrayElements( src, src_array_elements, 0 );
//
//	return srcSize;
//}
//
//JNIEXPORT jshort JNICALL
//Encode711u(JNIEnv *env, jobject thiz, jbyteArray dst, jshortArray src, jint srcSize)
//{
//	srcSize >>= 1;
//
//	jshort *src_array_elements = (jshort *)env->GetShortArrayElements(src, NULL);
//	jbyte *dst_array_elements = (jbyte *)env->GetByteArrayElements(dst, NULL);
//
//	jbyte* end = dst_array_elements + srcSize;
//	jshort temp;
//    jshort* pSrc = src_array_elements;
//    jbyte* pDst = dst_array_elements;
//
//	while (pDst < end) {
//		temp = *pSrc++;
//		*pDst++ = ULawEncode(temp);
//	}
//
//	env->ReleaseByteArrayElements( dst, dst_array_elements, 0 );
//	env->ReleaseShortArrayElements( src, src_array_elements, 0 );
//
//	return srcSize;
//}

JNIEXPORT jshort JNICALL
Encode711a(JNIEnv *env, jobject thiz, jbyteArray dst, jbyteArray src, jint srcSize)
{
    srcSize >>= 1;

    jbyte *src_array_elements = (jbyte *)env->GetByteArrayElements(src, NULL);
    jbyte *dst_array_elements = (jbyte *)env->GetByteArrayElements(dst, NULL);

    jbyte* end = dst_array_elements + srcSize;
    jshort temp;
    jbyte* pSrc = src_array_elements;
    jbyte* pDst = dst_array_elements;

    while (dst_array_elements < end) {
        temp = pSrc[0]&0x00ff | ((pSrc[1]<<8)&0xff00);
        *pDst++ = ALawEncode(temp);
        pSrc += 2;
    }

    env->ReleaseByteArrayElements( dst, dst_array_elements, 0 );
    env->ReleaseByteArrayElements( src, src_array_elements, 0 );

    return srcSize;
}

JNIEXPORT jshort JNICALL
Encode711u(JNIEnv *env, jobject thiz, jbyteArray dst, jbyteArray src, jint srcSize)
{
    srcSize >>= 1;

    jbyte *src_array_elements = (jbyte *)env->GetByteArrayElements(src, NULL);
    jbyte *dst_array_elements = (jbyte *)env->GetByteArrayElements(dst, NULL);

    jbyte* end = dst_array_elements + srcSize;
    jshort temp;
    jbyte* pSrc = src_array_elements;
    jbyte* pDst = dst_array_elements;

    while (pDst < end) {
        temp = pSrc[0]&0x00ff | ((pSrc[1]<<8)&0xff00);
        *pDst++ = ULawEncode(temp);
        pSrc += 2;
    }

    env->ReleaseByteArrayElements( dst, dst_array_elements, 0 );
    env->ReleaseByteArrayElements( src, src_array_elements, 0 );

    return srcSize;
}

static JNINativeMethod gMethods[] = {
	{	"Decode711a", "([S[BI)S", 	(void*)Decode711a	},
	{	"Decode711u", "([S[BI)S", 	(void*)Decode711u	},
	{	"Decode711u2", "([B[BI)S", 	(void*)Decode711u2	},
	{	"Encode711a", "([B[BI)S", 	(void*)Encode711a	},
    {	"Encode711u", "([B[BI)S", 	(void*)Encode711u	},
//    {	"Encode711a", "([B[SI)S", 	(void*)Encode711a	},
//    {	"Encode711u", "([B[SI)S", 	(void*)Encode711u	},
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
