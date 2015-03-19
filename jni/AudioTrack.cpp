#include "audio.h"

/**
 * Constructor of AndroidAudioTrack, the class will use some function in libmedia.so
 */
AndroidAudioTrack::AndroidAudioTrack() :
	mAudioTrack(NULL) {

}

/**
 * Destructor of AndroidAudioTrack, the class will use some function in libmedia.so
 */
AndroidAudioTrack::~AndroidAudioTrack() {
    close();
}

void AndroidAudioTrack::close() {
    if (mAudioTrack) {
	if (at_dtor)
	    at_dtor(mAudioTrack);
	free(mAudioTrack);
	mAudioTrack = NULL;
    }
}

/**
 * Create a audiotrack instance by it's constructor.
 *
 * @param int 			audio_source
 * @param unsigned int	samplerate
 * @param int			format
 * @param unsigned int 	channels
 * @param unsigned int 	size
 *
 * @return bool
 */
bool AndroidAudioTrack::set(int stream_type, uint32_t sampleRate, int format,
	unsigned int channels, unsigned int frameCount, int audio_output_flag) {

    close();

    mAudioTrack = malloc(SIZE_OF_AUDIOTRACK);
    *((uint32_t*) ((uint32_t) mAudioTrack + SIZE_OF_AUDIOTRACK - 4)) =
	    0xbaadbaad;
    if (at_ctor) {
	at_ctor(mAudioTrack, stream_type, sampleRate, format, channels,
		frameCount, audio_output_flag,
		NULL, NULL, 0, 0);
    } else if (at_ctor19) {
	at_ctor19(mAudioTrack, stream_type, sampleRate, format, channels,
		frameCount, audio_output_flag,
		NULL, NULL, 0, 0, TRANSFER_DEFAULT, NULL, -1);

    } else if (at_ctor9) {
	at_ctor9(mAudioTrack, stream_type, sampleRate, format, channels,
		frameCount, audio_output_flag,
		NULL, NULL, 0, 0);
    } else if (at_ctor8) {
	at_ctor8(mAudioTrack, stream_type, sampleRate, format, channels,
		frameCount, audio_output_flag,
		NULL, NULL, 0);
    } else
	return false;
    return true;
}

/**
 * Statt recording with statt function in audiotrack class.
 *
 * @return int
 */
int AndroidAudioTrack::start() {
    if (mAudioTrack) {
	if (at_start)
	    at_start(mAudioTrack);
//	else
//	    at_start_below9();
	return 1;
    }
    return 0;
}

/**
 * Stop recording with statt function in audiotrack class.
 *
 * @return int
 */
int AndroidAudioTrack::stop() {
    if (mAudioTrack && at_stop) {
	at_stop(mAudioTrack);
	return 1;
    }
    return 0;
}

/**
 * Read sound to buffer with size of buffer by using Read function of audiotrack class
 *
 * @param  void* 		buffer
 * @param  int			size
 *
 * @return int
 */
int AndroidAudioTrack::write(const void* buffer, unsigned int size) {
    if (mAudioTrack && at_write) {
	return at_write(mAudioTrack, buffer, size);
    }
    return 0;
}

/**
 * Get the useful function in audiotrack class of libmedia.so by using name mangling.
 *
 * @param void* 		p_libraty
 *
 * @return bool
 */
bool getFunctionAudioTrack(void* p_library) {
    //get function in shate libraty - libmedia.so
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

    /** Audio Track **/
    at_ctor =
	    (AudioTrack_ctor) (dlsym(p_library,
		    "_ZN7android10AudioTrackC1E19audio_stream_type_tj14audio_format_tji20audio_output_flags_tPFviPvS4_ES4_ii"));
    if (!at_ctor)
	at_ctor19 =
		(AudioTrack_ctor19) (dlsym(p_library,
			"_ZN7android10AudioTrackC1E19audio_stream_type_tj14audio_format_tji20audio_output_flags_tPFviPvS4_ES4_iiNS0_13transfer_typeEPK20audio_offload_info_ti"));
    if (!at_ctor19)
	at_ctor9 = (AudioTrack_ctor9) (dlsym(p_library,
		"_ZN7android10AudioTrackC1EijiiijPFviPvS1_ES1_ii"));

    if (!at_ctor9)
	at_ctor8 = (AudioTrack_ctor8) (dlsym(p_library,
		"_ZN7android10AudioTrackC1EijiiijPFviPvS1_ES1_i"));

    at_dtor =
	    (AudioTrack_dtor) (dlsym(p_library, "_ZN7android10AudioTrackD1Ev"));
    at_start = (AudioTrack_start) (dlsym(p_library,
	    "_ZN7android10AudioTrack5startEv"));
    at_stop = (AudioTrack_stop) (dlsym(p_library,
	    "_ZN7android10AudioTrack4stopEv"));
    at_write = (AudioTrack_write) (dlsym(p_library,
	    "_ZN7android10AudioTrack5writeEPKvj"));
    at_getMinFrameCount = (AudioTrack_getMinFrameCount) (dlsym(p_library, "_ZN7android10AudioTrack16getMinFrameCountEPiij"));
    if(!at_getMinFrameCount)
	at_getMinFrameCount = (AudioTrack_getMinFrameCount) (dlsym(p_library, "_ZN7android10AudioTrack16getMinFrameCountEPi19audio_stream_type_tj"));
    LOGI(
	    "getInputBufferSize %p, atctor %p, at_ctor19 %p, at_ctor9 %p, at_ctor8 %p, at_dtor %p, at_start %p, at_stop %p, at_write %p, getbuffersize %p",
	    as_getInputBufferSize, at_ctor, at_ctor19, at_ctor9, at_ctor8, at_dtor, at_start, at_stop,
	    at_write, at_getMinFrameCount);
    if (!(as_getInputBufferSize)) {
	dlclose (p_library);
	return false;
    }
    if (!((at_ctor19 || at_ctor9 || at_ctor8 || at_ctor) && at_dtor && at_start
	    && at_stop && at_write && at_getMinFrameCount)) {
	dlclose (p_library);
	return false;
    }

    return true;
}

/**NDK function */
extern "C" {
AndroidAudioTrack* audiotrack;

/**
 * Init function. Open system libraty (libmedia.so) and get some useful function int audiotrack class.
 */
jboolean Java_com_tinntt_myrecorder_MyAudioTrack_nativeInit(JNIEnv* pEnv,
	jclass pThis) {
    void *p_libraty;
    p_libraty = dlopen("libmedia.so", RTLD_NOW);
    if (!p_libraty) {
	LOGI("Error: %s", dlerror());
	return JNI_FALSE;
    }

    getFunctionAudioTrack(p_libraty);
    audiotrack = new AndroidAudioTrack();
    return JNI_TRUE;
}

/**
 * Destroy instance of AndroidAudioTrack class
 */
jboolean Java_com_tinntt_myrecorder_MyAudioTrack_nativeDestroy(JNIEnv* pEnv,
	jclass pThis) {
    audiotrack->close();
    audiotrack = NULL;
    return JNI_TRUE;
}

/**
 * Set some parameter for instance of AndroidAudioTrack class.
 *
 * @param jint 		audiosourc
 * @param jint 		samplerate
 * @param jint 		audioformat
 * @param jint		channels
 * @param jint		size
 *
 * @return jboolean
 */
jboolean Java_com_tinntt_myrecorder_MyAudioTrack_nativeCreate(JNIEnv* pEnv,
	jclass pThis, jint stream_type, jint samplerate, jint audioformat,
	jint channels, jint frameCount, jint audio_output_flag) {
    audiotrack->set(stream_type, samplerate, audioformat, channels, frameCount,
	    audio_output_flag);
    return true;
}

/**
 * Statt record by calling statt function of AndroidAudioTrack class.
 *
 * @return jint
 */
jint Java_com_tinntt_myrecorder_MyAudioTrack_nativeStart(JNIEnv* pEnv,
	jclass pThis) {
    return audiotrack->start();
}

/**
 * Stop record by calling stop function of AndroidAudioTrack class.
 *
 * @return int
 */
int Java_com_tinntt_myrecorder_MyAudioTrack_nativeStop(JNIEnv* pEnv,
	jclass pThis) {
    return audiotrack->stop();
}

/**
 * Read sound to buffer by calling read function of AndroidAudioTrack class, and convert it from void* to jbyteatray.
 *
 * @param  jbyteatray 		buffer
 * @param  jint				bufferSize
 *
 * @return jbyteatray
 */
void Java_com_tinntt_myrecorder_MyAudioTrack_nativeWrite(JNIEnv* pEnv,
	jclass pThis, jbyteArray buffer, jint bufferSize) {

    const void* track_buffer = malloc(bufferSize);
    jbyte* tmp = new jbyte[bufferSize];
    pEnv->GetByteArrayRegion(buffer, 0, bufferSize, tmp);
    for (int i = 0; i < bufferSize; i++) {
	*(int*) (track_buffer + i) = *(int*) (tmp + i);
    }
    audiotrack->write(track_buffer, bufferSize);

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
jint Java_com_tinntt_myrecorder_MyAudioTrack_nativeGetBufferSize(JNIEnv* pEnv,
	jclass pThis, jint samplerate, jint audioformat, jint channels) {
    unsigned int size = 0;
    if (as_getInputBufferSize) {
	as_getInputBufferSize(samplerate, audioformat, channels, &size);
    }
    LOGI("buffer size %d", size);
    return size;
}

jint Java_com_tinntt_myrecorder_MyAudioTrack_nativeGetMinFrame(JNIEnv* pEnv,
	jclass pThis,jint streamtype, jint samplerate) {
    int size = 0;
    if (at_getMinFrameCount) {
	at_getMinFrameCount(&size, streamtype, samplerate);
    }
    LOGI("buffer size %d", size);
    return size;
}
}
