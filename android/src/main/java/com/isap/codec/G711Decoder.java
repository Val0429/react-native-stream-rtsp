package com.isap.codec;

public class G711Decoder {

	public static native short Decode711a(short[] dst, byte[] src, int size);
	public static native short Decode711u(short[] dst, byte[] src, int size);
	public static native short Decode711u2(byte[] dst, byte[] src, int size);

	public static native short Encode711a(byte[] dst, byte[] src, int size);
	public static native short Encode711u(byte[] dst, byte[] src, int size);

//	public static native short Encode711a(byte[] dst, short[] src, int size);
//	public static native short Encode711u(byte[] dst, short[] src, int size);
	
	static {
		System.loadLibrary("G711Decoder");
	}
}
