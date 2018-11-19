package com.isap.codec;

import android.graphics.Bitmap;

import com.isap.CodecDef;

public class VideoDecoder {

	private int _handleDecoder = 0;
	private Bitmap _bitmap = null;
	private Object _lock = new Object();

	// jni definition for decoder
	public native int MediaDecoderOpen(int nCodecType);

	public native void MediaDecoderClose(int nObj);

	public native void MediaDecoderDecodeFrame(int nObj, byte[] array,
			int arraySize, Bitmap bitmap);

	public native int MediaDecoderDecodeData(int nObj, byte[] array,
			int arraySize);

	public native int MediaDecoderGetLastDecodedFrame(int nObj, Bitmap bitmap);

	public native int MediaDecoderGetLastVideoWidth(int nObj);

	public native int MediaDecoderGetLastVideoHeight(int nObj);

	public native String hello();

	static {
		try {
			System.loadLibrary("DecoderInterface");
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	/*
	 * codecType = 0 => Unknown
	 * codecType = 1 => MJPEG
	 * codecType = 2 => MPEG4
	 * codecType = 3 => H264
	 * codecType = 4 => H265
	 */
	private int mappingType(CodecDef.MediaType type) {
		int id = 0;

		if (type == CodecDef.MediaType.VIDEO_MJPEG)
			id = 1;
		else if (type == CodecDef.MediaType.VIDEO_MPEG4)
			id = 2;
		else if (type == CodecDef.MediaType.VIDEO_H264)
			id = 3;
		else if (type == CodecDef.MediaType.VIDEO_H265)
			id = 4;
		return id;
	}
	
	public Bitmap decodeVideo(byte[] frameData, CodecDef.MediaType type) {
		synchronized (_lock) {
			try {
				if (_handleDecoder == 0)
					_handleDecoder = MediaDecoderOpen( mappingType(type) );

				int width = 0, height = 0;

				if (MediaDecoderDecodeData(_handleDecoder, frameData,
						frameData.length) == 0) {
					MediaDecoderClose(_handleDecoder);
					_handleDecoder = 0;
//					_bitmap.recycle();
					_bitmap = null;
					return null;
				}

				width = MediaDecoderGetLastVideoWidth(_handleDecoder);
				height = MediaDecoderGetLastVideoHeight(_handleDecoder);

				if (width * height == 0)
					return null;

				if (_bitmap == null || _bitmap.isRecycled())
					_bitmap = Bitmap.createBitmap(width, height,
							Bitmap.Config.ARGB_8888);

				if (MediaDecoderGetLastDecodedFrame(_handleDecoder, _bitmap) == 0) {
					MediaDecoderClose(_handleDecoder);
					_handleDecoder = 0;
//					_bitmap.recycle();
					_bitmap = null;
					return null;
				}

			} catch (OutOfMemoryError e) {
				e.printStackTrace();
				System.gc();
				return null;
			} catch (Exception ex) {
				ex.printStackTrace();
				return null;
			}
		}
		return _bitmap;
	}

	public void destory() {
		synchronized (_lock) {
			try {
				if (_handleDecoder == 0)
					return;
				MediaDecoderClose(_handleDecoder);
				_handleDecoder = 0;
			} catch (Exception ex) {
				ex.printStackTrace();
			}
		}
	}
}
