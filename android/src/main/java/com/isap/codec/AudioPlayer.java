package com.isap.codec;


import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;

public class AudioPlayer {

	private AudioTrack _audioTrack = null;
	private int _audioMinBufSize = 0;
	private byte[] _audioBuffer;


	public AudioPlayer() {

	}
	
	public void playSoundBuffer(short[] data, int length, int rate, int bits, int channels) {
		if (_audioTrack == null) {
			int pcm_bits = bits==8? AudioFormat.ENCODING_PCM_8BIT : AudioFormat.ENCODING_PCM_16BIT;
			int min_buff = bits==8? rate : rate * 2;

			_audioTrack = new AudioTrack(AudioManager.STREAM_MUSIC,
											rate,
											AudioFormat.CHANNEL_OUT_MONO,
											pcm_bits,
											min_buff,
											AudioTrack.MODE_STREAM);

			float maxVol =AudioTrack.getMaxVolume();
			_audioTrack.setStereoVolume(1.0f, 1.0f);
//			_audioTrack.setStereoVolume(maxVol, maxVol);
//			_audioTrack.setVolume(maxVol);

			play();
		}

		_audioTrack.write(data, 0, length);
	}

	public void playSoundBuffer(byte[] data, int length, int rate, int bits, int channels) {
		if (_audioTrack == null) {
			int pcm_bits = bits==8? AudioFormat.ENCODING_PCM_8BIT : AudioFormat.ENCODING_PCM_16BIT;
			int min_buff = bits==8? rate : rate * 2;

			_audioTrack = new AudioTrack(AudioManager.STREAM_MUSIC,
											rate,
											AudioFormat.CHANNEL_OUT_MONO,
											pcm_bits,
											min_buff,
											AudioTrack.MODE_STREAM);

			float maxVol =AudioTrack.getMaxVolume();
			_audioTrack.setStereoVolume(1.0f, 1.0f);
//			_audioTrack.setStereoVolume(maxVol, maxVol);
//			_audioTrack.setVolume(maxVol);

			play();
		}
		
		_audioTrack.write(data, 0, length);
	}
	
	public void play() {
		if (_audioTrack == null)
			return;

		if (_audioTrack.getPlayState() == AudioTrack.PLAYSTATE_STOPPED) {
			_audioTrack.play();
		} else if (_audioTrack.getPlayState() == AudioTrack.PLAYSTATE_PAUSED) {
			_audioTrack.play();
		}
	}

	public void pauseAllSounds() {
		if (_audioTrack == null)
			return;

		if (_audioTrack.getPlayState() == AudioTrack.PLAYSTATE_PLAYING)
			_audioTrack.pause();
	}

	public void stopAllSounds() {
		if (_audioTrack == null)
			return;

		_audioTrack.stop();
		_audioTrack.release();
	}

	public byte[] getAudioBuff() {
		return _audioBuffer;
	}
}
