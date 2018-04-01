#pragma once
#pragma region 依赖
// 必要头文件
#include <thread>
#include <iostream>
#include "WaveRecorder.h"
#include "windows.h"
// MSC头文件
#include "msp_cmn.h"		//	Mobile Speech Platform 公用头文件
#include "msp_types.h"		//	Mobile Speech Platform 枚举类型头文件
#include<msp_errors.h>
//#include "msp_errors.h"	//	Mobile Speech Platform 错误码定义
#include "qisr.h"			//	讯飞语音识别头文件
#include "qtts.h"			//	讯飞语音合成头文件
using namespace std;

#pragma endregion

/* 回调函数 */
typedef void(__stdcall *CharacterHandler)(string fragment, string rsltAdded);

class SpeechRecognizer
{
public:
	SpeechRecognizer(CharacterHandler callback_CH);				// 构造,登陆
	~SpeechRecognizer();										// 析构，登出
	static int ServerOperationAlert;
	static int SegmentationStatus;
	static int RecognitionStatus;
	static const char* SessionID;
	// todo : 识别结果传回片段handler
	static bool InService;							// 语音识别模块运行状态
	bool Start();									// 开始一次新的语音识别
	void Stop();									// 手动停止此次语音识别
private:
	static void CALLBACK onRec(array <char, CHUNCK_SIZE> ChunkData, UINT ChunksCount, bool bIsLast);
	static DWORD WINAPI receiver(LPVOID lpParameter);
	static bool stop;
public:
	int ret;										// 已弃用|句柄与返回值
	static string Result;
private:
	WaveRecorder Recorder;
	static CharacterHandler callback_CH;
	string session_begin_params = "sub = iat, domain = iat, language = zh_ch, accent = mandarin, aue = aue=speex-wb;7, sample_rate = 16000, result_type = plain, result_encoding = gb2312";
};
