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

import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.os.Bundle;
import android.support.v7.app.ActionBarActivity;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.Toast;

public class MainActivity extends ActionBarActivity {

	int freq = 8000, size;
	boolean isStop = false;
	byte[] buffer;
	Thread t;
	Button recordBtn, stopBtn;
	MyAudioRecord myAudioRecord;
	MyAudioTrack myAudioTrack;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		recordBtn = (Button) findViewById(R.id.recordBtn);
		stopBtn = (Button) findViewById(R.id.stopBtn);

		recordBtn.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {
				// TODO Auto-generated method stub
				if (Init()) {
					t = new Thread(new Runnable() {

						@Override
						public void run() {
							// TODO Auto-generated method stub
							while (!isStop) {
								buffer = myAudioRecord.read(buffer, 2048);
								myAudioTrack.write(buffer, buffer.length);
							}
						}
					});
					t.start();
				} else
					Toast.makeText(getApplicationContext(),
							"Can't start audio record", Toast.LENGTH_LONG)
							.show();
			}

		});

		stopBtn.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {
				// TODO Auto-generated method stub

				if (myAudioRecord != null) {
					t.interrupt();
					isStop = true;
					myAudioRecord.stop();
				}
				if (myAudioTrack != null) {
					myAudioTrack.stop();
				}
			}
		});

	}

	private boolean Init() {
		isStop = false;
		int frameCountTrack = MyAudioTrack.getMinFrame(
				AudioManager.STREAM_MUSIC, freq);
		int frameCountRecord = MyAudioRecord.getFrameCount(freq,
				MyAudioFormat.AUDIO_FORMAT_PCM_16_BIT,
				1);
		myAudioRecord = new MyAudioRecord(MyAudioFormat.AUDIO_SOURCE_MIC, freq,
				MyAudioFormat.AUDIO_FORMAT_PCM_16_BIT,
				MyAudioFormat.AUDIO_CHANNEL_IN_MONO, frameCountRecord);
		myAudioTrack = new MyAudioTrack(AudioManager.STREAM_MUSIC, freq,
				MyAudioFormat.AUDIO_FORMAT_PCM_16_BIT,
				4, frameCountTrack,
				0);

		if (myAudioRecord.start() == 0)
			return false;
		myAudioTrack.start();
		buffer = new byte[2048];
		return true;
	}
}
