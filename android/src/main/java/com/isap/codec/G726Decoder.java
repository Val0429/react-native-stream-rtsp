package com.isap.codec;

public class G726Decoder {
	
	public static native void init();
	public static native int DecodeG726_16( byte[] buf, int buf_len, short[] output );
	public static native int DecodeG726_24( byte[] buf, int buf_len, short[] output );
	public static native int DecodeG726_32( byte[] buf, int buf_len, short[] output );
	public static native int DecodeG726_40( byte[] buf, int buf_len, short[] output );
	
	static {
		System.loadLibrary("G726Decoder");
	}
	
}
