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

#include "jni.h"

#include <dlfcn.h>
#include <stdlib.h>
#include <android/log.h>

#define  LOG_TAG    "my recorder"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

/**Type to transfer buffer, i use TRANSFER_DEFAULT as a default parameter for construtor */
enum transfer_type {
	TRANSFER_DEFAULT, // not specified explicitly; determine from other parameters
	TRANSFER_CALLBACK,  // callback EVENT_MORE_DATA
	TRANSFER_OBTAIN, // FIXME deprecated: call obtainBuffer() and releaseBuffer()
	TRANSFER_SYNC,      // synchronous read()
};

/**A bitmask of acourstic values. It enables AGC, NS, IIR */
enum record_flags {
	RECORD_AGC_ENABLE = 0x0001,
	RECORD_NS_ENABLE = 0x0002,
	RECORD_IIR_ENABLE = 0x0004,
};

/**Input mode, use AUDIO_INPUT_FLAG_NONE as default parameter in constructor */
typedef enum {
	AUDIO_INPUT_FLAG_NONE = 0x0,        // no attributes
	AUDIO_INPUT_FLAG_FAST = 0x1, // prefer an input that supports "fast tracks"
} audio_input_flags_t;

/** Events used by AudioRecord callback function (callback_t).
 *
 * to keep in sync with frameworks/base/media/java/android/media/AudioRecord.java
 */
enum sync_event_t {
	SYNC_EVENT_SAME = -1, SYNC_EVENT_NONE = 0, SYNC_EVENT_PRESENTATION_COMPLETE,

	SYNC_EVENT_CNT,
};

extern "C" {

#define SIZE_OF_AUDIORECORD 512


/**
 * Helper method to get an input size of buffer for one second record, it's depend on sample rate, audio format, channels mask.
 *
 *  name mangling for all android's version:
 * 		_ZN7android11AudioSystem18getInputBufferSizeEj14audio_format_tjPj
 * 		_ZN7android11AudioSystem18getInputBufferSizeEjiiPj
 * 		_ZN7android11AudioSystem18getInputBufferSizeEj14audio_format_tiPj
 *
 * @param 	unsigned int  		sample_rate
 * @param 	int 				audio_format
 * @param 	unsigned int  		channels mask
 * @param 	int*				size
 * @return 	int 				size of buffer
 */
typedef int (*AudioSystem_getInputBufferSize)(unsigned int, int, unsigned int,
		unsigned int*);
/** Audio Record **/
/**
 * Constructor of AudioRecord use for API 19
 *
 *  name mangling for API 19:
 * 		_ZN7android11AudioRecordC1E14audio_source_tj14audio_format_tjiPFviPvS3_ES3_iiNS0_13transfer_typeE19audio_input_flags_t
 *
 * @param 	audio_sourc_t  		audio_source
 * @param 	unsigned int 		sample_rate
 * @param 	audio_format_t  	format
 * @param 	unsigned int  		channels
 * @param	int					size
 * @param	callback_function	cbf
 * @param	void*				user
 * @param	int					notificationFrames
 * @param	int 				sessionId
 * @param	transfer_type		transfer_type
 * @param	audio_input_flags_t	input_flags
 */
typedef void (*AudioRecord_ctor19)(void*, int, unsigned int, int, unsigned int,
		int, void (*)(int, void *, void *), void*, int, int, transfer_type,
		int);

/**
 * Constructor of AudioRecord use for above API 17
 *
 *  name mangling for above 19:
 * 		_ZN7android11AudioRecordC1E14audio_source_tj14audio_format_tjiPFviPvS3_ES3_ii
 *
 * @param 	audio_sourc_t  		audio_source
 * @param 	unsigned int 		sample_rate
 * @param 	audio_format_t  	format
 * @param 	unsigned int  		channels
 * @param	int					size
 * @param	callback_function	cbf
 * @param	void*				user
 * @param	int					notificationFrames
 * @param	int 				sessionId
 */
typedef void (*AudioRecord_ctor17)(void*, int, unsigned int, int, unsigned int,
		int, void (*)(int, void *, void *), void*, int, int);

/**
 * Constructor of AudioRecord use for API 16
 *
 *  name mangling for API 16:
 * 		_ZN7android11AudioRecordC1E14audio_source_tj14audio_format_tjiNS0_12record_flagsEPFviPvS4_ES4_ii
 *
 * @param 	audio_sourc_t  		audio_source
 * @param 	unsigned int 		sample_rate
 * @param 	audio_format_t  	format
 * @param 	unsigned int  		channels
 * @param	int					size
 * @param 	record_flags 		record_flags
 * @param	callback_function	cbf
 * @param	void*				user
 * @param	int					notificationFrames
 * @param	int 				sessionId
 */
typedef void (*AudioRecord_ctor16)(void*, int, unsigned int, int, unsigned int,
		int, record_flags, void (*)(int, void *, void *), void*, int, int);

/**
 * Constructor of AudioRecord use for above API 9
 *
 *  name mangling for above API 9:
 * 		_ZN7android11AudioRecordC1EijijijPFviPvS1_ES1_ii
 *
 * @param 	audio_sourc_t  		audio_source
 * @param 	unsigned int 		sample_rate
 * @param 	audio_format_t  	format
 * @param 	unsigned int  		channels
 * @param	int					size
 * @param 	int 		 		record_flags
 * @param	callback_function	cbf
 * @param	void*				user
 * @param	int					notificationFrames
 * @param	int 				sessionId
 */
typedef void (*AudioRecord_ctor9)(void*, int, unsigned int, int, unsigned int,
		int, unsigned int, void (*)(int, void *, void *), void*, int, int);

/**
 * Constructor of AudioRecord use for API 8
 *
 *  name mangling for API 8:
 * 		_ZN7android11AudioRecordC1EijijijPFviPvS1_ES1_i
 *
 * @param 	audio_sourc_t  		audio_source
 * @param 	unsigned int 		sample_rate
 * @param 	audio_format_t  	format
 * @param 	unsigned int  		channels
 * @param	int					size
 * @param 	int 		 		record_flags
 * @param	callback_function	cbf
 * @param	void*				user
 * @param	int					notificationFrames
 */
//_ZN7android11AudioRecordC1EijijijPFviPvS1_ES1_i
typedef void (*AudioRecord_ctor8)(void*, int, unsigned int, int, unsigned int,
		int, unsigned int, void (*)(int, void *, void *), void*, int);

/**
 * Destructor of AudioRecord
 *
 *  name mangling:
 * 		_ZN7android11AudioRecordD1Ev
 *
 */
typedef void (*AudioRecord_dtor)(void*);

/**
 * Start record function
 *
 *  name mangling:
 * 		_ZN7android11AudioRecord5startENS0_12sync_event_tEi
 *
 *@param sync_event_t 	event
 *@param int			trigger_session
 */

typedef int (*AudioRecord_start)(void*, int, int);

/**
 * Start record function use for API below 9
 *
 *  name mangling:
 * 		_ZN7android11AudioRecord5startEv
 */
typedef int (*AudioRecord_start_below9)();

/**
 * Stop record function
 *
 *  name mangling:
 * 		_ZN7android11AudioRecord4stopEv
 */
typedef void (*AudioRecord_stop)(void*);

/**
 * Read sound to buffer with size of buffer.
 *
 *  name mangling:
 * 		_ZN7android11AudioRecord4readEPvj
 *
 * @param void* 		buffer
 * @param unsigned int	size
 */
typedef int (*AudioRecord_read)(void*, void*, unsigned int);
typedef int (*AudioRecord_getMinFrameCount)( int*, unsigned int, int, int);
static AudioSystem_getInputBufferSize as_getInputBufferSize;

static AudioRecord_ctor19 ar_ctor19; // Available for android 19
static AudioRecord_ctor17 ar_ctor17; // Available for android above 17 below 19
static AudioRecord_ctor16 ar_ctor16; // Available for android 16
static AudioRecord_ctor9 ar_ctor9; // Available for android above 9 below 16
static AudioRecord_ctor8 ar_ctor8; // Available for android above 8 below 9
static AudioRecord_dtor ar_dtor;
static AudioRecord_start ar_start;
static AudioRecord_start_below9 ar_start_below9; // Available for android below 9
static AudioRecord_stop ar_stop;
static AudioRecord_read ar_read;
static AudioRecord_getMinFrameCount  ar_getMinFrameCount;
class AndroidAudioRecord {
private:
	void* mAudioRecord; //Instance of audiorecord
public:
	void* lbuffer;
	int size;

	AndroidAudioRecord();

	virtual ~AndroidAudioRecord();

	void close();
	static bool getFunction(void* p_library);
	/**Initialize the audiorecord */
	bool set(int audio_source, uint32_t sampleRate, int format,
			unsigned int channels, unsigned int size);

	/**start record */
	int start();

	/**stop record */
	int stop();

	/**push resource to buffer */
	int read(void* buffer, int size);

};

/** Audio Track **/

#define SIZE_OF_AUDIOTRACK 512
/**4.4**/
//AudioTrack::AudioTrack(audio_stream_type_t streamType, uint32_t sampleRate,
//	audio_format_t format, audio_channel_mask_t channelMask, int frameCount,
//	audio_output_flags_t flags, callback_t cbf, void* user,
//	int notificationFrames, int sessionId, transfer_type transferType,
//	const audio_offload_info_t *offloadInfo, int uid)
typedef void (*AudioTrack_ctor19)(void*, int, unsigned int, int, int, int, int,
	void (*)(int, void *, void *), void*, int, int, transfer_type,
	const void*, int);
/**2.3**/
//AudioTrack::AudioTrack(int streamType, uint32_t sampleRate, int format,
//	int channels, int frameCount, uint32_t flags, callback_t cbf,
//	void* user, int notificationFrames, int sessionId)
typedef void (*AudioTrack_ctor9)(void*, int, unsigned int, int, int, int,
	unsigned int, void (*)(int, void *, void *), void*, int, int);
/**2.2**/
//AudioTrack::AudioTrack(int streamType, uint32_t sampleRate, int format,
//	int channels, int frameCount, uint32_t flags, callback_t cbf,
//	void* user, int notificationFrames)
typedef void (*AudioTrack_ctor8)(void*, int, unsigned int, int, int, int, unsigned int,
	void (*)(int, void *, void *), void*, int);
/** another **/
//AudioTrack::AudioTrack(audio_stream_type_t streamType, uint32_t sampleRate,
//	audio_format_t format, audio_channel_mask_t channelMask, int frameCount,
//	audio_output_flags_t flags, callback_t cbf, void* user,
//	int notificationFrames, int sessionId)
typedef void (*AudioTrack_ctor)(void*, int, unsigned int, int, int, int, int,
	void (*)(int, void *, void *), void*, int, int);

typedef void (*AudioTrack_dtor)(void*);

typedef int (*AudioTrack_start)(void*);

typedef int (*AudioTrack_stop)(void*);

typedef int (*AudioTrack_write)(void*, const void*, unsigned int);
typedef int (*AudioTrack_getMinFrameCount)( int*, int, unsigned int);
bool getFunctionAudioTrack();

static AudioTrack_ctor19 at_ctor19; // Available for android 19
static AudioTrack_ctor9 at_ctor9;
static AudioTrack_ctor8 at_ctor8;
static AudioTrack_ctor at_ctor;
static AudioTrack_dtor at_dtor;
static AudioTrack_start at_start;
static AudioTrack_stop at_stop;
static AudioTrack_write at_write;
static AudioTrack_getMinFrameCount at_getMinFrameCount;
class AndroidAudioTrack {
private:
	void* mAudioTrack; //Instance of audiorecord
public:
	AndroidAudioTrack();

	virtual ~AndroidAudioTrack();

	void close();

	/**Initialize the audiorecord */
	bool set(int stream_type, uint32_t sampleRate, int format,
		unsigned int channels, unsigned int frameCount, int audio_output_flag);

	/**start play */
	int start();

	/**stop play */
	int stop();

	/**push resource to buffer */
	int write(const void* buffer,unsigned int size);
};

}
