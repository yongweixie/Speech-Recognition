#include "WaveRecorder.h"
#include "conio.h"
#include"SpeechRecognizer.h"
#include"Vocalizer.h"
#include"TlRobot.hpp"
/*这是一个回调函数示例*/
void CALLBACK CharHandler(std::string fragment, std::string rsltAdded);
void VocalFeedback(const char*  sentence);
string Recognizersentence;//识别到的语句
string txt_feedback;//回答的语句
SpeechVolizer speech;
SpeechRecognizer Recognizer(CharHandler);
int main()
{
	while (true)
	{
		Recognizer.Stop();
		if (!Recognizer.Start())
		{
			cout << "start error";
		}
		Sleep(2000);
	}	
	system("pause");
}

/*这是一个回调函数示例*/

void CALLBACK CharHandler(std::string fragment, std::string resultAdd)
{
	
	const char* result = resultAdd.c_str();
	if (!resultAdd.empty())
	{
		
			//VocalFeedback(GetResult(fragment).c_str());
		cout << fragment << endl;
	}
}

void VocalFeedback(const char* sentence)
{
	Recognizer.Stop();
	txt_feedback = sentence;
	speech.Vocalizer(sentence,"xiaoyan");
}