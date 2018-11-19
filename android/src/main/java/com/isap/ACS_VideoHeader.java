package com.isap;

public class ACS_VideoHeader {

	public static final int VFCT_IVOP = 0;
	public static final int VFCT_PVOP = 1;
	public static final int VFCT_JPEG = 5;
	public static final int VFCT_H264_IFRM = 10;
	public static final int VFCT_H264_PFRM = 11;
	public static final int VFCT_H265_IFRM = 16;
	public static final int VFCT_H265_PFRM = 17;

	public static final int VIDEO_HEADER_LENGTH = 40;
	public static final int STREAM_VIDEO = 0xf5010000;
	
	// 4bytes * 7 + 2bytes * 4 + 1byte * 4 = 40
	public long 	lHdrID;
	public long 	lHdrLength;
	public long 	lDataLength;
	public long 	lSequenceNumber;
	public long 	lTimeSec;
	public long 	lTimeUSec;
	public long		lDataCheckSum;
	public int		iCodingType;
	public int		iFrameRate;
	public int 		iWidth;
	public int 		iHeight;
	public char		cMDBitmap;
	public char[]	cMDPowers = new char[3];
	
	
	public boolean setHeader( byte[] header ) {
		if (header.length < VIDEO_HEADER_LENGTH)
			return false;
		
		int i = 0;
		lHdrID			= (header[i++]&0xff) | (header[i++]&0xff)<<8 | (header[i++]&0xff)<<16 | (header[i++]&0xff)<<24;
		lHdrLength		= (header[i++]&0xff) | (header[i++]&0xff)<<8 | (header[i++]&0xff)<<16 | (header[i++]&0xff)<<24;
		lDataLength		= (header[i++]&0xff) | (header[i++]&0xff)<<8 | (header[i++]&0xff)<<16 | (header[i++]&0xff)<<24;
		lSequenceNumber	= (header[i++]&0xff) | (header[i++]&0xff)<<8 | (header[i++]&0xff)<<16 | (header[i++]&0xff)<<24;
		lTimeSec		= (header[i++]&0xff) | (header[i++]&0xff)<<8 | (header[i++]&0xff)<<16 | (header[i++]&0xff)<<24;
		lTimeUSec		= (header[i++]&0xff) | (header[i++]&0xff)<<8 | (header[i++]&0xff)<<16 | (header[i++]&0xff)<<24;
		lDataCheckSum	= (header[i++]&0xff) | (header[i++]&0xff)<<8 | (header[i++]&0xff)<<16 | (header[i++]&0xff)<<24;
		iCodingType		= (header[i++]&0xff) | (header[i++]&0xff)<<8;
		iFrameRate		= (header[i++]&0xff) | (header[i++]&0xff)<<8;
		iWidth			= (header[i++]&0xff) | (header[i++]&0xff)<<8;
		iHeight			= (header[i++]&0xff) | (header[i++]&0xff)<<8;
		cMDBitmap		= (char) (header[i++]&0xff);
		cMDPowers[0]	= (char) (header[i++]&0xff);
		cMDPowers[1]	= (char) (header[i++]&0xff);
		cMDPowers[2]	= (char) (header[i++]&0xff);
		
		return true;
	}
	
}
