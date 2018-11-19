package com.isap;

public class ACS_AudioHeader {

	public static final int AFMT_MS_ADPCM = 0;
	public static final int AFMT_MU_LAW = 1;
	public static final int AFMT_A_LAW = 2;
	public static final int AFMT_IMA_ADPCM = 4;
	public static final int AFMT_U8 = 8;
	public static final int AFMT_S16_LE = 0x10;
	public static final int AFMT_S16_BE = 0x20;
	public static final int AFMT_S8 = 0x40;
	public static final int AFMT_U16_LE = 0x80;
	public static final int AFMT_U16_BE = 0x100;
	public static final int AFMT_MPEG = 0x200;
	public static final int AFMT_AC3 = 0x400;
	public static final int AFMT_AMR = 0x800;
	public static final int AFMT_AAC = 0x1000;
	public static final int AFMT_ALAW = 0X2000;

	public static final int AUDIO_HEADER_LENGTH = 40;
	public static final int STREAM_AUDIO = 0xf6010000;

	// 4bytes * 8 + 2bytes * 4 = 40
	public long	lHdrID;
	public long	lHdrLength;
	public long	lDataLength;
	public long	lSequenceNumber;
	public long	lTimeSec;
	public long	lTimeUSec;
	public long	lDataCheckSum;
	public int	iFormat;
	public int	iChannels;
	public int	iSampleRate;
	public int	iSampleBits;
	public long lReserved;

	public boolean setHeader( byte[] header ) {
		if (header.length < AUDIO_HEADER_LENGTH)
			return false;
		
		int i = 0;
		lHdrID			= (header[i++]&0xff) | (header[i++]&0xff)<<8 | (header[i++]&0xff)<<16 | (header[i++]&0xff)<<24;
		lHdrLength		= (header[i++]&0xff) | (header[i++]&0xff)<<8 | (header[i++]&0xff)<<16 | (header[i++]&0xff)<<24;
		lDataLength		= (header[i++]&0xff) | (header[i++]&0xff)<<8 | (header[i++]&0xff)<<16 | (header[i++]&0xff)<<24;
		lSequenceNumber	= (header[i++]&0xff) | (header[i++]&0xff)<<8 | (header[i++]&0xff)<<16 | (header[i++]&0xff)<<24;
		lTimeSec		= (header[i++]&0xff) | (header[i++]&0xff)<<8 | (header[i++]&0xff)<<16 | (header[i++]&0xff)<<24;
		lTimeUSec		= (header[i++]&0xff) | (header[i++]&0xff)<<8 | (header[i++]&0xff)<<16 | (header[i++]&0xff)<<24;
		lDataCheckSum	= (header[i++]&0xff) | (header[i++]&0xff)<<8 | (header[i++]&0xff)<<16 | (header[i++]&0xff)<<24;
		iFormat			= (header[i++]&0xff) | (header[i++]&0xff)<<8;
		iChannels		= (header[i++]&0xff) | (header[i++]&0xff)<<8;
		iSampleRate		= (header[i++]&0xff) | (header[i++]&0xff)<<8;
		iSampleBits		= (header[i++]&0xff) | (header[i++]&0xff)<<8;
		lReserved		= (header[i++]&0xff) | (header[i++]&0xff)<<8 | (header[i++]&0xff)<<16 | (header[i++]&0xff)<<24;
		
		return true;
	}
	
	public byte[] makeHeader() {
		byte[] header = new byte[ AUDIO_HEADER_LENGTH ];
		int i = 0;

//		if (true) 
		{
			header[i++] = (byte)((lHdrID	  ) & 0xff);
			header[i++] = (byte)((lHdrID >>  8) & 0xff);
			header[i++] = (byte)((lHdrID >> 16) & 0xff);
			header[i++] = (byte)((lHdrID >> 24) & 0xff);

			header[i++] = (byte)((lHdrLength	  ) & 0xff);
			header[i++] = (byte)((lHdrLength >>  8) & 0xff);
			header[i++] = (byte)((lHdrLength >> 16) & 0xff);
			header[i++] = (byte)((lHdrLength >> 24) & 0xff);

			header[i++] = (byte)((lDataLength	   ) & 0xff);
			header[i++] = (byte)((lDataLength >>  8) & 0xff);
			header[i++] = (byte)((lDataLength >> 16) & 0xff);
			header[i++] = (byte)((lDataLength >> 24) & 0xff);

			header[i++] = (byte)((lSequenceNumber	   ) & 0xff);
			header[i++] = (byte)((lSequenceNumber >>  8) & 0xff);
			header[i++] = (byte)((lSequenceNumber >> 16) & 0xff);
			header[i++] = (byte)((lSequenceNumber >> 24) & 0xff);

			header[i++] = (byte)((lTimeSec	  	) & 0xff);
			header[i++] = (byte)((lTimeSec >>  8) & 0xff);
			header[i++] = (byte)((lTimeSec >> 16) & 0xff);
			header[i++] = (byte)((lTimeSec >> 24) & 0xff);

			header[i++] = (byte)((lTimeUSec	  	 ) & 0xff);
			header[i++] = (byte)((lTimeUSec >>  8) & 0xff);
			header[i++] = (byte)((lTimeUSec >> 16) & 0xff);
			header[i++] = (byte)((lTimeUSec >> 24) & 0xff);

			header[i++] = (byte)((lDataCheckSum	  	 ) & 0xff);
			header[i++] = (byte)((lDataCheckSum >>  8) & 0xff);
			header[i++] = (byte)((lDataCheckSum >> 16) & 0xff);
			header[i++] = (byte)((lDataCheckSum >> 24) & 0xff);

			header[i++] = (byte)((iFormat      ) & 0xff);
			header[i++] = (byte)((iFormat >>  8) & 0xff);

			header[i++] = (byte)((iChannels      ) & 0xff);
			header[i++] = (byte)((iChannels >>  8) & 0xff);

			header[i++] = (byte)((iSampleRate      ) & 0xff);
			header[i++] = (byte)((iSampleRate >>  8) & 0xff);

			header[i++] = (byte)((iSampleBits      ) & 0xff);
			header[i++] = (byte)((iSampleBits >>  8) & 0xff);

			header[i++] = (byte)((lReserved	  	 ) & 0xff);
			header[i++] = (byte)((lReserved >>  8) & 0xff);
			header[i++] = (byte)((lReserved >> 16) & 0xff);
			header[i++] = (byte)((lReserved >> 24) & 0xff);
		}
//		else
//		{
//			header[i++] = (byte)((lHdrID >> 24) & 0xff);
//			header[i++] = (byte)((lHdrID >> 16) & 0xff);
//			header[i++] = (byte)((lHdrID >>  8) & 0xff);
//			header[i++] = (byte)((lHdrID	  ) & 0xff);
//
//			header[i++] = (byte)((lHdrLength >> 24) & 0xff);
//			header[i++] = (byte)((lHdrLength >> 16) & 0xff);
//			header[i++] = (byte)((lHdrLength >>  8) & 0xff);
//			header[i++] = (byte)((lHdrLength	  ) & 0xff);
//			
//			header[i++] = (byte)((lDataLength >> 24) & 0xff);
//			header[i++] = (byte)((lDataLength >> 16) & 0xff);
//			header[i++] = (byte)((lDataLength >>  8) & 0xff);
//			header[i++] = (byte)((lDataLength	   ) & 0xff);
//			
//			header[i++] = (byte)((lSequenceNumber >> 24) & 0xff);
//			header[i++] = (byte)((lSequenceNumber >> 16) & 0xff);
//			header[i++] = (byte)((lSequenceNumber >>  8) & 0xff);
//			header[i++] = (byte)((lSequenceNumber	   ) & 0xff);
//			
//			header[i++] = (byte)((lTimeSec >> 24) & 0xff);
//			header[i++] = (byte)((lTimeSec >> 16) & 0xff);
//			header[i++] = (byte)((lTimeSec >>  8) & 0xff);
//			header[i++] = (byte)((lTimeSec	  	) & 0xff);
//			
//			header[i++] = (byte)((lTimeUSec >> 24) & 0xff);
//			header[i++] = (byte)((lTimeUSec >> 16) & 0xff);
//			header[i++] = (byte)((lTimeUSec >>  8) & 0xff);
//			header[i++] = (byte)((lTimeUSec	  	 ) & 0xff);
//			
//			header[i++] = (byte)((lDataCheckSum >> 24) & 0xff);
//			header[i++] = (byte)((lDataCheckSum >> 16) & 0xff);
//			header[i++] = (byte)((lDataCheckSum >>  8) & 0xff);
//			header[i++] = (byte)((lDataCheckSum	  	 ) & 0xff);
//			
//			header[i++] = (byte)((iFormat >>  8) & 0xff);
//			header[i++] = (byte)((iFormat      ) & 0xff);
//			
//			header[i++] = (byte)((iChannels >>  8) & 0xff);
//			header[i++] = (byte)((iChannels      ) & 0xff);
//			
//			header[i++] = (byte)((iSampleRate >>  8) & 0xff);
//			header[i++] = (byte)((iSampleRate      ) & 0xff);
//			
//			header[i++] = (byte)((iSampleBits >>  8) & 0xff);
//			header[i++] = (byte)((iSampleBits      ) & 0xff);
//			
//			header[i++] = (byte)((lReserved >> 24) & 0xff);
//			header[i++] = (byte)((lReserved >> 16) & 0xff);
//			header[i++] = (byte)((lReserved >>  8) & 0xff);
//			header[i++] = (byte)((lReserved	  	 ) & 0xff);
//		}
		
		return header;
	}
}
