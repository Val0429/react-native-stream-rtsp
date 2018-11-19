

#ifdef __cplusplus
extern "C" {
#endif


#include <jni.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>

#include <android/bitmap.h>
#include <android/log.h>
#include <strings.h>

#include <syscall.h>
#include <unistd.h>

#define  LOG_TAG    "FFMPEGSample"
#define  LOGE(...)  __android_log_print( ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__ )
//#define	 LOG_FFMPEG

#define COLOR_FORMAT PIX_FMT_BGR32

static void fill_bitmap( AndroidBitmapInfo* info, void *pixels, AVFrame *pFrame )
{
	uint8_t *frameLine;
	int yy = 0;
	int xx = 0;

	for( yy = 0; yy < info->height; yy++ )
	{
		uint8_t*  line = (uint8_t*)pixels;
		frameLine = (uint8_t *)pFrame->data[0] + ( yy * pFrame->linesize[0] );

		for( xx = 0; xx < info->width; xx++ )
		{
			int out_offset = xx * 4;
			int in_offset = xx * 3;
			line[ out_offset ] = frameLine[ in_offset ];
			line[ out_offset + 1 ] = frameLine[ in_offset + 1 ];
			line[ out_offset + 2 ] = frameLine[ in_offset + 2 ];
			line[ out_offset + 3 ] = 0;
		}
		pixels = (char*)pixels + info->stride;
	}
}

void ffmpeg_log_callback(void *ptr, int level, const char *fmt, va_list vargs)
{
   static char message[8192];
   const char *module = NULL;

   // Comment back in to filter only "important" messages
   /*if (level > AV_LOG_WARNING)
      return;*/

   // Get module name
    if (ptr)
    {
        AVClass *avc = *(AVClass**) ptr;
        module = avc->item_name(ptr);
    }

   // Create the actual message
    vsnprintf(message, sizeof(message), fmt, vargs);

   LOGE( "ffmpeg: %s", message );
}

typedef struct
{
	AVFormatContext *pFormatCtx;
	AVCodecContext 	*pCodecCtx;
	AVFrame 		*pFrame;
	AVFrame 		*pFrameRGB;
	AVCodec			*pAvcodec;
	struct SwsContext *img_convert_ctx;
	int				lastDecodedVideoWidth;
	int				lastDecodedVideoHeight;
	void			*buffer;
	int 			codecType;
}VideoDecodeContext;



#define JNIREG_CLASS "com/isap/codec/VideoDecoder"

JNIEXPORT jstring JNICALL native_hello(JNIEnv *env, jclass clazz)
{
	printf("hello in c native code./n");
	return (*env)->NewStringUTF(env, "hello world returned.");
}


JNIEXPORT jint JNICALL native_MediaDecoderOpen( JNIEnv * env, jobject this, jint codecType )
{
	av_register_all();

	VideoDecodeContext *pContext = calloc( 1, sizeof(VideoDecodeContext));

	pContext->pFormatCtx = NULL;
	pContext->pCodecCtx = NULL;
	pContext->pFrame = NULL;
	pContext->pFrameRGB = NULL;
	pContext->pAvcodec = NULL;
	pContext->buffer = NULL;
	pContext->codecType = codecType;

	/*
	 * codecType = 0 => Unknown
	 * codecType = 1 => MJPEG
	 * codecType = 2 => MPEG4
	 * codecType = 3 => H264
	 * codecType = 4 => H265
	 */
	if( codecType == 1 )
		pContext->pAvcodec = avcodec_find_decoder( AV_CODEC_ID_MJPEG );
	else if( codecType == 2 )
		pContext->pAvcodec = avcodec_find_decoder( AV_CODEC_ID_MPEG4 );
	else if( codecType == 3 )
        pContext->pAvcodec = avcodec_find_decoder( AV_CODEC_ID_H264 );
    else if (codecType == 4)
        pContext->pAvcodec = avcodec_find_decoder( AV_CODEC_ID_HEVC );
	else
		return 0;

	pContext->pCodecCtx = avcodec_alloc_context3( pContext->pAvcodec );
	pContext->pCodecCtx->pix_fmt = COLOR_FORMAT;
	pContext->pFrame = avcodec_alloc_frame();
	int nRet = avcodec_open2( pContext->pCodecCtx, pContext->pAvcodec, 0 );

	if( nRet == 0 )
		return (jint)(int)pContext;
	else
	{
		if( pContext )
		{
			if( pContext->buffer )
				free( pContext->buffer );

			if( pContext->pCodecCtx )
				avcodec_close( pContext->pCodecCtx );
			free( pContext );
		}
	}

#ifdef LOG_FFMPEG
	av_log_set_callback(&ffmpeg_log_callback);
#endif

	return 0;
}

JNIEXPORT void JNICALL native_MediaDecoderClose( JNIEnv * env, jobject this, jint myDecoderObj )
{
	VideoDecodeContext *pContext = (VideoDecodeContext*)myDecoderObj;

	if( pContext )
	{
		if( pContext->buffer )
		{
			free( pContext->buffer );
			pContext->buffer = NULL;
		}

		if( pContext->pCodecCtx )
		{
			avcodec_close( pContext->pCodecCtx );
			pContext->pCodecCtx = NULL;
		}

		if( pContext->img_convert_ctx )
		{
			sws_freeContext( pContext->img_convert_ctx );
			pContext->img_convert_ctx = NULL;
		}

		free( pContext );

	}

#ifdef LOG_FFMPEG
	av_log_set_callback( 0 );
#endif
}

JNIEXPORT void JNICALL native_MediaDecoderDecodeFrame( JNIEnv * env, jobject this, jint myDecoderObj, jbyteArray VideoDataArray, jint VideoDataArraySize, jobject bitmap )
{

	AndroidBitmapInfo  info;
	void*              pixels;
	int                ret;
	int err;
	int i;
	int got_gop = 0;
	AVPacket packet;
	static struct SwsContext *img_convert_ctx;
	int64_t seek_target;

	int nObjAddr = myDecoderObj;

	VideoDecodeContext *pDecoderObj = (VideoDecodeContext*)nObjAddr;

	if( ( ret = AndroidBitmap_getInfo( env, bitmap, &info ) ) < 0 )
	{
		return;
	}


	if( ( ret = AndroidBitmap_lockPixels( env, bitmap, &pixels ) ) < 0 )
	{
	}


	jbyte *b = (jbyte *)(*env)->GetByteArrayElements( env, VideoDataArray, NULL );


	av_init_packet( &packet );
    packet.data = b;
    packet.size = VideoDataArraySize;
	avcodec_decode_video2( pDecoderObj->pCodecCtx, pDecoderObj->pFrame, &got_gop, &packet );


	if (pDecoderObj->codecType == 3 || pDecoderObj->codecType == 4) {
		if( got_gop <= 0 ) {
			packet.data = NULL;
			packet.size = 0;
			avcodec_decode_video2(pDecoderObj->pCodecCtx, pDecoderObj->pFrame, &got_gop,
								  &packet);
		}
	}

	if( got_gop <= 0 )
		return;

	if( got_gop )
	{
			img_convert_ctx = sws_getContext( pDecoderObj->pCodecCtx->width,
											pDecoderObj->pCodecCtx->height,
											pDecoderObj->pCodecCtx->pix_fmt,

											// target w x h
											pDecoderObj->pCodecCtx->width,
											pDecoderObj->pCodecCtx->height,
											// target format
											COLOR_FORMAT,


											SWS_BICUBIC,
											NULL, NULL, NULL);

		if( pDecoderObj-> img_convert_ctx == NULL )
		{
			return;
		}

		if( pDecoderObj->buffer == NULL && pDecoderObj->pFrameRGB == NULL )
		{
			pDecoderObj->pFrameRGB = avcodec_alloc_frame();
			int numBytes = avpicture_get_size( COLOR_FORMAT, pDecoderObj->pCodecCtx->width, pDecoderObj->pCodecCtx->height );
			pDecoderObj->buffer = (uint8_t *)av_malloc( numBytes * sizeof( uint8_t ) );
			avpicture_fill( (AVPicture *)pDecoderObj->pFrameRGB, pDecoderObj->buffer, COLOR_FORMAT, pDecoderObj->pCodecCtx->width, pDecoderObj->pCodecCtx->height );
		}


		sws_scale( 	img_convert_ctx,
					(const uint8_t* const*)pDecoderObj->pFrame->data,
					pDecoderObj->pFrame->linesize,
					0,
					pDecoderObj->pCodecCtx->height,
					pDecoderObj->pFrameRGB->data,
					pDecoderObj->pFrameRGB->linesize );

		memcpy( (uint8_t*)pixels, (uint8_t *)pDecoderObj->pFrameRGB->data[0], info.stride * info.height );
	}

	av_free_packet( &packet );
	(*env)->ReleaseByteArrayElements( env, VideoDataArray, b, 0 );

	AndroidBitmap_unlockPixels( env, bitmap );
}


JNIEXPORT jint JNICALL native_MediaDecoderDecodeData( JNIEnv * env, jobject this, jint myDecoderObj, jbyteArray VideoDataArray, jint VideoDataArraySize )
{
	AndroidBitmapInfo  info;
	void*              pixels;
	int                ret;
	int err;
	int i;
	int got_gop = 0;
	AVPacket packet;
	int64_t seek_target;

    ////////////////////////////////
    jint tid = syscall(__NR_gettid);
    ////////////////////////////////

	int nObjAddr = myDecoderObj;

	VideoDecodeContext *pDecoderObj = (VideoDecodeContext*)nObjAddr;

	if (!pDecoderObj)
		return 1;

	jbyte *b = (jbyte *)(*env)->GetByteArrayElements( env, VideoDataArray, NULL );

	av_init_packet( &packet );
    packet.data = b;
    packet.size = VideoDataArraySize;

	avcodec_decode_video2( pDecoderObj->pCodecCtx, pDecoderObj->pFrame, &got_gop, &packet );

    if (pDecoderObj->codecType == 3 || pDecoderObj->codecType == 4) {
        if( got_gop <= 0 ) {
            packet.data = NULL;
            packet.size = 0;
            avcodec_decode_video2(pDecoderObj->pCodecCtx, pDecoderObj->pFrame, &got_gop, &packet);
        }
    }


	if( got_gop )
	{
		if (pDecoderObj->lastDecodedVideoWidth != pDecoderObj->pCodecCtx->width || pDecoderObj->lastDecodedVideoHeight != pDecoderObj->pCodecCtx->height)
		{
			if (pDecoderObj->lastDecodedVideoWidth != 0 && pDecoderObj->lastDecodedVideoHeight != 0)
			{
				// if we get here means resolution has been changed in somewhere.
				av_free_packet( &packet );
				(*env)->ReleaseByteArrayElements( env, VideoDataArray, b, 0 );
				return 0;
			}
		}

		pDecoderObj->lastDecodedVideoWidth = pDecoderObj->pCodecCtx->width;
		pDecoderObj->lastDecodedVideoHeight = pDecoderObj->pCodecCtx->height;
		pDecoderObj->img_convert_ctx = sws_getCachedContext( pDecoderObj->img_convert_ctx,
											pDecoderObj->pCodecCtx->width,
											pDecoderObj->pCodecCtx->height,
											pDecoderObj->pCodecCtx->pix_fmt,

											// target w x h
											pDecoderObj->pCodecCtx->width,
											pDecoderObj->pCodecCtx->height,
											// target format
											COLOR_FORMAT,


											SWS_BICUBIC,
											NULL, NULL, NULL);

		if( pDecoderObj->img_convert_ctx == NULL )
		{
			return 1;
		}

		if( pDecoderObj->buffer == NULL && pDecoderObj->pFrameRGB == NULL )
		{
			pDecoderObj->pFrameRGB = avcodec_alloc_frame();
			int numBytes = avpicture_get_size( COLOR_FORMAT, pDecoderObj->pCodecCtx->width, pDecoderObj->pCodecCtx->height );
			pDecoderObj->buffer = (uint8_t *)av_malloc( numBytes * sizeof( uint8_t ) );
			avpicture_fill( (AVPicture *)pDecoderObj->pFrameRGB, pDecoderObj->buffer, COLOR_FORMAT, pDecoderObj->pCodecCtx->width, pDecoderObj->pCodecCtx->height );
		}

		sws_scale( 	pDecoderObj->img_convert_ctx,
					(const uint8_t* const*)pDecoderObj->pFrame->data,
					pDecoderObj->pFrame->linesize,
					0,
					pDecoderObj->pCodecCtx->height,
					pDecoderObj->pFrameRGB->data,
					pDecoderObj->pFrameRGB->linesize );

	}

	av_free_packet( &packet );
	(*env)->ReleaseByteArrayElements( env, VideoDataArray, b, 0 );

	return 1;
}

JNIEXPORT int JNICALL native_MediaDecoderGetLastDecodedFrame( JNIEnv * env, jobject this, jint myDecoderObj, jobject bitmap )
{

	AndroidBitmapInfo  info;
	void*              pixels;
	int                ret;
	int err;
	int i;
	int frameFinished = 0;
	AVPacket packet;
	static struct SwsContext *img_convert_ctx;
	int64_t seek_target;

	int nObjAddr = myDecoderObj;

	VideoDecodeContext *pDecoderObj = (VideoDecodeContext*)nObjAddr;

	if( ( ret = AndroidBitmap_getInfo( env, bitmap, &info ) ) < 0 )
	{
		return 0;
	}

	if( ( ret = AndroidBitmap_lockPixels( env, bitmap, &pixels ) ) < 0 )
	{
        return 0;
	}
	else
	{
		memcpy( (uint8_t*)pixels, (uint8_t *)pDecoderObj->pFrameRGB->data[0], info.stride * info.height );
	}

	AndroidBitmap_unlockPixels( env, bitmap );

    return 1;
}

JNIEXPORT jint JNICALL native_MediaDecoderGetLastVideoWidth( JNIEnv * env, jobject this, jint myDecoderObj )
{
	VideoDecodeContext *pContext = (VideoDecodeContext*)myDecoderObj;
	if( pContext )
	{
		return (jint)(int)pContext->lastDecodedVideoWidth;
	}
	return 0;
}

JNIEXPORT jint JNICALL native_MediaDecoderGetLastVideoHeight( JNIEnv * env, jobject this, jint myDecoderObj )
{
	VideoDecodeContext *pContext = (VideoDecodeContext*)myDecoderObj;
	if( pContext )
	{
		return (jint)(int)pContext->lastDecodedVideoHeight;
	}
	return 0;
}

static JNINativeMethod gMethods[] = {
	{ "hello", "()Ljava/lang/String;", (void*)native_hello },
	{ "MediaDecoderOpen", "(I)I", (void*)native_MediaDecoderOpen },
	{ "MediaDecoderClose", "(I)V", (void*)native_MediaDecoderClose },
	{ "MediaDecoderDecodeFrame", "(I[BILandroid/graphics/Bitmap;)V", (void*)native_MediaDecoderDecodeFrame },
	{ "MediaDecoderDecodeData", "(I[BI)I", (void*)native_MediaDecoderDecodeData },
	{ "MediaDecoderGetLastDecodedFrame", "(ILandroid/graphics/Bitmap;)I", (void*)native_MediaDecoderGetLastDecodedFrame },
	{ "MediaDecoderGetLastVideoWidth", "(I)I", (void*)native_MediaDecoderGetLastVideoWidth },
	{ "MediaDecoderGetLastVideoHeight", "(I)I", (void*)native_MediaDecoderGetLastVideoHeight },
};

static int registerNativeMethods(JNIEnv* env, const char* className,
        JNINativeMethod* gMethods, int numMethods)
{
	jclass clazz;
	clazz = (*env)->FindClass(env, className);
	if (clazz == NULL) {
		return JNI_FALSE;
	}
	if ((*env)->RegisterNatives(env, clazz, gMethods, numMethods) < 0) {
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

	if ((*vm)->GetEnv(vm, (void**) &env, JNI_VERSION_1_4) != JNI_OK) {
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
