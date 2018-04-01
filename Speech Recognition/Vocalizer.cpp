#include"Vocalizer.h"
#include"SDKwavefile.h"
SpeechVolizer::SpeechVolizer()
{

}

SpeechVolizer::~SpeechVolizer()
{
	pSource->Stop();                                                // 关闭音源
	pSource->FlushSourceBuffers();                                  // 缓冲区清空(可选)
}
int SpeechVolizer::text_to_speech(const char* src_text, const char* des_path, const char* params)
{
	
	if (NULL == src_text || NULL == des_path)
	{
		std::cout << "params is error!"<<endl;
		return ret;
	}
	fopen_s(&fp, des_path, "wb");
	if (NULL == fp)
	{
		std::cout << "open" << des_path << "error" << endl;
		return ret;
	}
	/* 开始合成 */
	sessionID = QTTSSessionBegin(params, &ret);
	if (MSP_SUCCESS != ret)
	{
		fclose(fp);  return ret;
	}
	ret = QTTSTextPut(sessionID, src_text, (unsigned int)strlen(src_text), NULL);
	if (MSP_SUCCESS != ret)
	{
		QTTSSessionEnd(sessionID, "TextPutError");
		fclose(fp);
		return ret;
	}
	fwrite(&wav_hdr, sizeof(wav_hdr), 1, fp); //添加wav音频头，使用采样率为16000
	while (1)
	{
		/* 获取合成音频 */
		const void* data = QTTSAudioGet(sessionID, &audio_len, &synth_status, &ret);
		if (MSP_SUCCESS != ret)
			break;
		if (NULL != data)
		{
			fwrite(data, audio_len,1, fp);
			wav_hdr.data_size += audio_len; //计算data_size大小
		}
		if (MSP_TTS_FLAG_DATA_END == synth_status)
			break;
		Sleep(150); //防止频繁占用CPU
	}

	if (MSP_SUCCESS != ret)
	{
		QTTSSessionEnd(sessionID, "AudioGetError");
		fclose(fp);
		return ret;
	}
	/* 修正wav文件头数据的大小 */
	wav_hdr.size_8 += wav_hdr.data_size + (sizeof(wav_hdr) - 8);

	/* 将修正过的数据写回文件头部,音频文件为wav格式 */
	fseek(fp, 4, 0);
	fwrite(&wav_hdr.size_8, sizeof(wav_hdr.size_8), 1, fp); //写入size_8的值
	fseek(fp, 40, 0); //将文件指针偏移到存储data_size值的位置
	fwrite(&wav_hdr.data_size, sizeof(wav_hdr.data_size), 1, fp); //写入data_size的值
	fclose(fp);
	fp = NULL;
	/* 合成完毕 */
	ret = QTTSSessionEnd(sessionID, "Normal");
	if (MSP_SUCCESS != ret)
	{
		cout << "enderror" << endl;
	}

	
	return ret;
}
int  SpeechVolizer::filetospeech()
{
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (FAILED(XAudio2Create(&XAudioEngine))) return FALSE;
	if (FAILED(XAudioEngine->CreateMasteringVoice(&pmaster)))return FALSE;

	CWaveFile waveFile;
	HRESULT hr = waveFile.Open(L"temp.wav", NULL, WAVEFILE_READ);//加载文件  
	if (FAILED(hr))
		return 0;

	WAVEFORMATEX *waveFormat = waveFile.GetFormat();//获取文件格式  

	DWORD size = waveFile.GetSize();//获取文件的大小  
	BYTE *pData = new BYTE[size];//申请内存空间，用于保存数据  
	hr = waveFile.Read(pData, size, &size);//读取文件内容  
	if (FAILED(hr))
		return 0;
	if (FAILED(XAudioEngine->CreateSourceVoice(&pSource, waveFormat)))
	{
		std::cout << "error" << endl; return FALSE;
	}

	pSource->Start();
	sBuffer.pAudioData = pData;           // 合成音频
	sBuffer.AudioBytes = size;                      // 一次性载入音频缓冲区
	if (FAILED(pSource->SubmitSourceBuffer(&sBuffer)))
		return 0;     // 装载音频
					  // 等待播放完毕或者打断
	for (pSource->GetState(&pState); pState.BuffersQueued; pSource->GetState(&pState))
		Sleep(10);
	waveFile.Close();//关闭文件  
	delete[]pData;
	pData = NULL;
}
void SpeechVolizer::Vocalizer(const char* src_text, string Speachname)
{
	int         ret = MSP_SUCCESS;
	const char* login_params = "appid = 59c06558, work_dir = .";//登录参数,appid与msc库绑定,请勿随意改动
																/*
																* rdn:           合成音频数字发音方式
																* volume:        合成音频的音量
																* pitch:         合成音频的音调
																* speed:         合成音频对应的语速
																* voice_name:    合成发音人
																* sample_rate:   合成音频采样率
																* text_encoding: 合成文本编码格式
																*
																*/
	string begin_param_string = "voice_name=" + Speachname + "," + "text_encoding = gb2312, sample_rate = 16000, speed = 50, volume = 50, pitch = 50, rdn = 1";
	const char* session_begin_params = begin_param_string.c_str();
	const char* filename = "temp.wav"; //合成的语音文件名称
	const char* text = src_text; //合成文本
																							 /* 用户登录 */
	//ret = MSPLogin(NULL, NULL, login_params); //第一个参数是用户名，第二个参数是密码，第三个参数是登录参数，用户名和密码可在http://www.xfyun.cn注册获取
	//if (MSP_SUCCESS != ret)
	//{
	//	cout << "登陆失败";
	//}

	/* 文本合成 */
	std:: cout << "开始合成...." << endl;
	ret = text_to_speech(src_text, filename, session_begin_params);
	filetospeech();
	if (MSP_SUCCESS != ret)
	{
		//printf("text_to_speech failed, error code: %d.\n", ret);
		cout << "合成失败";
	}
	//system("pause");
	//MSPLogout(); //退出登录
	remove("temp.wav");
}

