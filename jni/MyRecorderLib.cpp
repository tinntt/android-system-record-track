#include "audio.h"

/**
 * Constructor of AndroidAudioRecord, the class will use some function in libmedia.so
 */
AndroidAudioRecord::AndroidAudioRecord() :
	mAudioRecord(NULL) {

}

/**
 * Destructor of AndroidAudioRecord, the class will use some function in libmedia.so
 */
AndroidAudioRecord::~AndroidAudioRecord() {
    close();
}
void AndroidAudioRecord::close() {
    if (mAudioRecord) {
	if (ar_dtor)
	    ar_dtor(mAudioRecord);
	free(mAudioRecord);
	mAudioRecord = NULL;
    }
}

/**
 * Create a AudioRecord instance by it's constructor.
 *
 * @param int 			audio_source
 * @param unsigned int	samplerate
 * @param int			format
 * @param unsigned int 	channels
 * @param unsigned int 	size
 *
 * @return bool
 */
bool AndroidAudioRecord::set(int audio_source, uint32_t sampleRate, int format,
	unsigned int channels, unsigned int size) {

    close();

    mAudioRecord = malloc(SIZE_OF_AUDIORECORD);
    *((uint32_t*) ((uint32_t) mAudioRecord + SIZE_OF_AUDIORECORD - 4)) =
	    0xbaadbaad;
    if (ar_ctor19) {
	ar_ctor19(mAudioRecord, audio_source, sampleRate, format, channels,
		size, NULL, NULL, 0, 0, TRANSFER_DEFAULT,
		AUDIO_INPUT_FLAG_NONE);
    } else if (ar_ctor17)
	ar_ctor17(mAudioRecord, audio_source, sampleRate, format, channels,
		size, NULL, NULL, 0, 0);
    else if (ar_ctor16)
	ar_ctor16(mAudioRecord, audio_source, sampleRate, format, channels,
		size, (record_flags) 0, NULL, NULL, 0, 0);
    else if (ar_ctor9)
	ar_ctor9(mAudioRecord, audio_source, sampleRate, format, channels, size,
		0, NULL, NULL, 0, 0);
    else if (ar_ctor8)
	ar_ctor8(mAudioRecord, audio_source, sampleRate, format, channels, size,
		0, NULL, NULL, 0);
    else
	return false;
    return true;
}

/**
 * Start recording with start function in AudioRecord class.
 *
 * @return int
 */
int AndroidAudioRecord::start() {
    if (mAudioRecord) {
	if (ar_start)
	    ar_start(mAudioRecord, SYNC_EVENT_NONE, 0);
	else
	    ar_start_below9();
	return 1;
    }
    return 0;
}

/**
 * Stop recording with start function in AudioRecord class.
 *
 * @return int
 */
int AndroidAudioRecord::stop() {
    if (mAudioRecord && ar_stop) {
	ar_stop(mAudioRecord);
	return 1;
    }
    return 0;
}

/**
 * Read sound to buffer with size of buffer by using Read function of AudioRecord class
 *
 * @param  void* 		buffer
 * @param  int			size
 *
 * @return int
 */
int AndroidAudioRecord::read(void* buffer, int size) {
    if (mAudioRecord && ar_read) {
	return ar_read(mAudioRecord, buffer, size);
    }
    return 0;
}

/**
 * Get the useful function in AudioRecord class of libmedia.so by using name mangling.
 *
 * @param void* 		p_library
 *
 * @return bool
 */
bool getFunctionAudioRecord(void* p_library) {
    //get function in share library - libmedia.so
    as_getInputBufferSize =
	    (AudioSystem_getInputBufferSize) (dlsym(p_library,
		    "_ZN7android11AudioSystem18getInputBufferSizeEj14audio_format_tjPj"));

    if (!as_getInputBufferSize)
	as_getInputBufferSize =
		(AudioSystem_getInputBufferSize) (dlsym(p_library,
			"_ZN7android11AudioSystem18getInputBufferSizeEjiiPj"));

    if (!as_getInputBufferSize)
	as_getInputBufferSize =
		(AudioSystem_getInputBufferSize) (dlsym(p_library,
			"_ZN7android11AudioSystem18getInputBufferSizeEj14audio_format_tiPj"));
    /** Audio Record **/
    ar_ctor19 =
	    (AudioRecord_ctor19) (dlsym(p_library,
		    "_ZN7android11AudioRecordC1E14audio_source_tj14audio_format_tjiPFviPvS3_ES3_iiNS0_13transfer_typeE19audio_input_flags_t"));

    if (!ar_ctor19)
	ar_ctor17 =
		(AudioRecord_ctor17) (dlsym(p_library,
			"_ZN7android11AudioRecordC1E14audio_source_tj14audio_format_tjiPFviPvS3_ES3_ii"));

    if (!ar_ctor17)
	ar_ctor16 =
		(AudioRecord_ctor16) (dlsym(p_library,
			"_ZN7android11AudioRecordC1E14audio_source_tj14audio_format_tjiNS0_12record_flagsEPFviPvS4_ES4_ii"));

    if (!ar_ctor16)
	ar_ctor9 = (AudioRecord_ctor9) (dlsym(p_library,
		"_ZN7android11AudioRecordC1EijijijPFviPvS1_ES1_ii"));

    if (!ar_ctor9)
	ar_ctor8 = (AudioRecord_ctor8) (dlsym(p_library,
		"_ZN7android11AudioRecordC1EijijijPFviPvS1_ES1_i"));

    ar_dtor = (AudioRecord_dtor) (dlsym(p_library,
	    "_ZN7android11AudioRecordD1Ev"));

    ar_start = (AudioRecord_start) (dlsym(p_library,
	    "_ZN7android11AudioRecord5startENS_11AudioSystem12sync_event_tEi"));

    if (!ar_start)
	ar_start_below9 = (AudioRecord_start_below9) (dlsym(p_library,
		"_ZN7android11AudioRecord5startEv"));

    ar_stop = (AudioRecord_stop) (dlsym(p_library,
	    "_ZN7android11AudioRecord4stopEv"));
    ar_read = (AudioRecord_read) (dlsym(p_library,
	    "_ZN7android11AudioRecord4readEPvj"));

    ar_getMinFrameCount = (AudioRecord_getMinFrameCount) (dlsym(p_library,"_ZN7android11AudioRecord16getMinFrameCountEPijii"));
    if(!ar_getMinFrameCount)
	ar_getMinFrameCount = (AudioRecord_getMinFrameCount) (dlsym(p_library,"_ZN7android11AudioRecord16getMinFrameCountEPij14audio_format_ti"));
    if(!ar_getMinFrameCount)
    	ar_getMinFrameCount = (AudioRecord_getMinFrameCount) (dlsym(p_library,"_ZN7android11AudioRecord16getMinFrameCountEPij14audio_format_tj"));
    // check function.
    LOGI(
    	    "getInputBufferSize %p", ar_getMinFrameCount);
    if (!(as_getInputBufferSize)) {
	dlclose(p_library);
	return false;
    }
    if (!((ar_ctor19 || ar_ctor16 || ar_ctor17 || ar_ctor8 || ar_ctor9)
	    && ar_dtor && (ar_start || ar_start_below9) && ar_stop && ar_read && ar_getMinFrameCount)) {
	dlclose(p_library);
	return false;
    }
    return true;
}

/**NDK function */
extern "C" {
AndroidAudioRecord* audiorecord;

/**
 * Init function. Open system library (libmedia.so) and get some useful function int AudioRecord class.
 */
jboolean Java_com_tinntt_myrecorder_MyAudioRecord_nativeInit(JNIEnv* pEnv,
	jclass pThis) {
    void *p_library;
    p_library = dlopen("libmedia.so", RTLD_NOW);
    if (!p_library) {
	LOGI("Error: %s", dlerror());
	return JNI_FALSE;
    }

    getFunctionAudioRecord(p_library);
    audiorecord = new AndroidAudioRecord();
    return JNI_TRUE;
}

/**
 * Destroy instance of AndroidAudioRecord class
 */
jboolean Java_com_tinntt_myrecorder_MyAudioRecord_nativeDestroy(JNIEnv* pEnv,
	jclass pThis) {
    audiorecord->close();
    audiorecord = NULL;
    return JNI_TRUE;
}

/**
 * Set some parameter for instance of AndroidAudioRecord class.
 *
 * @param jint 		audiosourc
 * @param jint 		samplerate
 * @param jint 		audioformat
 * @param jint		channels
 * @param jint		size
 *
 * @return jboolean
 */
jboolean Java_com_tinntt_myrecorder_MyAudioRecord_nativeCreate(JNIEnv* pEnv,
	jclass pThis, jint audiosource, jint samplerate, jint audioformat,
	jint channels, jint size) {
    audiorecord->set(audiosource, samplerate, audioformat, channels, size);
    return true;
}

/**
 * Start record by calling start function of AndroidAudioRecord class.
 *
 * @return jint
 */
jint Java_com_tinntt_myrecorder_MyAudioRecord_nativeStart(JNIEnv* pEnv,
	jclass pThis) {
    return audiorecord->start();
}

/**
 * Stop record by calling stop function of AndroidAudioRecord class.
 *
 * @return int
 */
int Java_com_tinntt_myrecorder_MyAudioRecord_nativeStop(JNIEnv* pEnv,
	jclass pThis) {
    free(audiorecord->lbuffer);
    return audiorecord->stop();
}
//_ZN7android10AudioTrackC1E
/**
 * Read sound to buffer by calling read function of AndroidAudioRecord class, and convert it from void* to jbyteArray.
 *
 * @param  jbyteArray 		buffer
 * @param  jint				bufferSize
 *
 * @return jbyteArray
 */
jbyteArray Java_com_tinntt_myrecorder_MyAudioRecord_nativeRead(JNIEnv* pEnv,
	jclass pThis, jbyteArray buffer, jint bufferSize) {
    audiorecord->lbuffer = malloc(bufferSize);
    audiorecord->size = bufferSize;

    audiorecord->read(audiorecord->lbuffer, bufferSize);

    jbyte* tmp = new jbyte[audiorecord->size];

    for (int i = 0; i < audiorecord->size; i++) {
	*(int*) (tmp + i) = *(int*) (audiorecord->lbuffer + i);
    }
    jbyteArray lbuffer = pEnv->NewByteArray(audiorecord->size);
    pEnv->SetByteArrayRegion(lbuffer, 0, audiorecord->size, tmp);
    free(tmp);
    return lbuffer;
}

/**
 * Helper method to get an input size of buffer for one second record, it's use the method of AudioSystem class.
 *
 * @param 	jint			samplerate
 * @param 	jint 			audioformat
 * @param 	jint  			channels
 *
 * @return 	unsiged int		size
 */
jint Java_com_tinntt_myrecorder_MyAudioRecord_nativeGetBufferSize(JNIEnv* pEnv,
	jclass pThis, jint samplerate, jint audioformat, jint channels) {
    unsigned int size = 0;
    if (as_getInputBufferSize) {
	as_getInputBufferSize(samplerate, audioformat, channels, &size);
    }
    LOGI("buffer size %d", size);
    return size;
}

jint Java_com_tinntt_myrecorder_MyAudioRecord_nativeGetMinFrame(JNIEnv* pEnv,
	jclass pThis, jint samplerate, jint audioformat, jint channels) {
    int size = 0;
    if (ar_getMinFrameCount) {
	ar_getMinFrameCount(&size, samplerate, audioformat, channels);
    }
    LOGI("buffer size %d", size);
    return size;
}
}
