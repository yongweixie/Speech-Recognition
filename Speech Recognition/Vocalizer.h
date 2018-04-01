#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <errno.h>
#include<iostream>
#include "qtts.h"
#include "msp_cmn.h"
#include "msp_errors.h"
#include <xaudio2.h>
#include<vector>
#pragma comment(lib,"xaudio2.lib") 

using namespace std;
class SpeechVolizer
{
public:
	SpeechVolizer();
	~SpeechVolizer();
	int SpeechVolizer::text_to_speech(const char* src_text, const char* des_path, const char* params);
	void Vocalizer(const char* src_text, string name);
	int  SpeechVolizer::filetospeech();
private:
	
	typedef struct _wave_pcm_hdr
	{
		char            riff[4];                // = "RIFF"
		int				size_8;                 // = FileSize - 8
		char            wave[4];                // = "WAVE"
		char            fmt[4];                 // = "fmt "
		int				fmt_size;				// = 下一个结构体的大小 : 16

		short int       format_tag;             // = PCM : 1
		short int       channels;               // = 通道数 : 1
		int				samples_per_sec;        // = 采样率 : 8000 | 6000 | 11025 | 16000
		int				avg_bytes_per_sec;      // = 每秒字节数 : samples_per_sec * bits_per_sample / 8
		short int       block_align;            // = 每采样点字节数 : wBitsPerSample / 8
		short int       bits_per_sample;        // = 量化比特数: 8 | 16

		char            data[4];                // = "data";
		int				data_size;              // = 纯数据长度 : FileSize - 44 
	} wave_pcm_hdr;
	wave_pcm_hdr default_wav_hdr =
	{
		{ 'R', 'I', 'F', 'F' },
		0,
		{ 'W', 'A', 'V', 'E' },
		{ 'f', 'm', 't', ' ' },
		16,
		1,
		1,
		16000,
		32000,
		2,
		16,
		{ 'd', 'a', 't', 'a' },
		0
	};



	int          ret = -1;
	FILE*        fp = NULL;
	const char*  sessionID = NULL;
	unsigned int audio_len = 0;
	wave_pcm_hdr wav_hdr = default_wav_hdr;
	int          synth_status = MSP_TTS_FLAG_STILL_HAVE_DATA;
	WAVEFORMATEX wfx;

	IXAudio2 * XAudioEngine = NULL;                                 // IXAduio2音频引擎
	IXAudio2MasteringVoice * pmaster = NULL;                        // 声音管理器
	IXAudio2SourceVoice * pSource = NULL;                           // 音源
	XAUDIO2_BUFFER sBuffer = {};                                    // 音源缓冲区
	XAUDIO2_VOICE_STATE pState = {};                                // 缓冲区状态
};

