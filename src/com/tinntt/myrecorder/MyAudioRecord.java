/*
 * Copyright 2014 Tín Nguyễn
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

package com.tinntt.myrecorder;

import android.util.Log;

public class MyAudioRecord {
	static {
		try{
			System.loadLibrary("MyRecorderLib");
		}
		catch(UnsatisfiedLinkError e){
			System.err.println("Native code library failed to load.\n" + e.toString());
		}
	}
	
	public MyAudioRecord(int audiosource, int samplerate, int audioformat,
			int channels,int size) {
		// TODO Auto-generated constructor stub		
		//int size = getBufferSize(samplerate, audioformat, channels);
		nativeCreate(audiosource, samplerate, audioformat, channels, size);	
	}

	public static int getFrameCount(int samplerate, int audioformat,
			int channels) {		
		nativeInit();
//		if (android.os.Build.VERSION.SDK_INT <= 16)
//			channels = 1;
		int result = nativeGetMinFrame(samplerate, audioformat, channels);
		return result;
	}

	public int start() {
		return nativeStart();
	}

	public int stop() {
		return nativeStop();
	}

	public byte[] read(byte[] buffer, int buffersize) {
		return nativeRead(buffer, buffersize);
	}

	public boolean destroy() {
		return nativeDestroy();
	}

	public static native boolean nativeInit();

	public static native boolean nativeDestroy();

	public static native int nativeStart();

	public static native int nativeCreate(int audiosource, int samplerate,
			int audioformat, int channels, int size);

	public static native int nativeStop();

	public static native byte[] nativeRead(byte[] buffer, int bufferSize);

	public static native int nativeGetBufferSize(int samplerate,
			int audioformat, int channels);
	public static native int nativeGetMinFrame(int samplerate,
			int audioformat, int channels);
}
