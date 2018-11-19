package com.isap.codec;

public class AACDecoder {

	public static native void init();
	public static native int peek( byte[] buf, int buf_len, short[] output );
	public static native int decode( byte[] buf, int buf_len, short[] output );
	public static native byte[] shortToByte(short[] buf);
	public static native String hello();
	public static native int decodeAAC( byte[] buf, int buf_len, byte[] output );
	public static native int getDecodedSize();

	public static native int getSampleRate();
	public static native int getChannels();
	public static native int getSampleBit();

	static {
		System.loadLibrary("AACDecoder");
	}
}
