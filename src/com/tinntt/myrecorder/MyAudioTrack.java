package com.tinntt.myrecorder;

import android.util.Log;

public class MyAudioTrack {
	static {
		try{
			System.loadLibrary("MyRecorderLib");
		}
		catch(UnsatisfiedLinkError e){
			System.err.println("Native code library failed to load.\n" + e.toString());
		}
	}
	
	public MyAudioTrack(int stream_type, int samplerate, int audioformat,
			int channels, int frameCount, int audio_output_flag) {
		// TODO Auto-generated constructor stub		
		//int size = getBufferSize(samplerate, audioformat, channels);
		nativeCreate(stream_type, samplerate, audioformat, channels, frameCount, audio_output_flag);	
	}

	public static int getMinFrame(int streamtype,
			int samplerate) {		
		nativeInit();
//		if (android.os.Build.VERSION.SDK_INT <= 16)
//			channels = 1;
		int result = nativeGetMinFrame(streamtype, samplerate);
		return result;
	}

	public int start() {
		return nativeStart();
	}

	public int stop() {
		return nativeStop();
	}

	public void write(byte[] buffer, int buffersize) {
		nativeWrite(buffer, buffersize);
	}

	public boolean destroy() {
		return nativeDestroy();
	}

	public static native boolean nativeInit();
	
	public static native boolean nativeDestroy();

	public static native int nativeStart();

	public static native int nativeCreate(int stream_type, int samplerate, int audioformat,
			int channels, int frameCount, int audio_output_flag);

	public static native int nativeStop();

	public static native void nativeWrite(byte[] buffer, int bufferSize);

	public static native int nativeGetBufferSize(int samplerate,
			int audioformat, int channels);
	
	public static native int nativeGetMinFrame(int streamtype,
			int samplerate);
}
