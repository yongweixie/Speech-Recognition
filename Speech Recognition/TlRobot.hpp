#pragma once
#include <iostream>
#include "curl\\curl.h"
#include "json\\json.h"
#include <string>
using namespace std;
#pragma comment(lib,"lib_json.lib")
#pragma comment(lib,"LIBCURL.LIB")
enum NetMode
{
	NM_UNKNOWN = -1,
	NM_POST = 0,
	NM_HTTPPOST,
	NM_GET,
	NM_PUT
};

struct NET_OPT
{
	std::string strUrl;
	NetMode mode;
	std::string file;
	std::string key;
	std::string text;
};
string UTF8ToGBK(const std::string& strUTF8)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, strUTF8.c_str(), strUTF8.length(), NULL, 0);
	WCHAR* wszGBK = new WCHAR[len + 1];
	memset(wszGBK, 0, len * 2 + 2);
	MultiByteToWideChar(CP_UTF8, 0, (const char*)strUTF8.c_str(), strUTF8.length(), wszGBK, len);

	len = WideCharToMultiByte(CP_ACP, 0, wszGBK, len, NULL, 0, NULL, NULL);
	char *szGBK = new char[len + 1];
	memset(szGBK, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, wszGBK, len, szGBK, len, NULL, NULL);
	std::string strTemp(szGBK);
	delete[]szGBK;
	delete[]wszGBK;
	return strTemp;
}
string GBKToUTF8(const std::string& strGBK)
{
	string strOutUTF8 = "";
	WCHAR * str1;
	int n = MultiByteToWideChar(CP_ACP, 0, strGBK.c_str(), strGBK.length(), NULL, 0);
	str1 = new WCHAR[n + 1];
	memset(str1, 0, (n + 1) * 2);
	MultiByteToWideChar(CP_ACP, 0, strGBK.c_str(), strGBK.length(), str1, n);
	int m = WideCharToMultiByte(CP_UTF8, 0, str1, n, NULL, 0, NULL, NULL);
	char * str2 = new char[m + 1];
	memset(str2, 0, m + 1);
	WideCharToMultiByte(CP_UTF8, 0, str1, n, str2, m, NULL, NULL);
	strOutUTF8 = str2;
	delete[]str1;
	str1 = NULL;
	delete[]str2;
	str2 = NULL;
	return strOutUTF8;
}
CURL* curl_test_init()
{
	CURLcode cd = curl_global_init(CURL_GLOBAL_DEFAULT);
	if (cd != CURLE_OK)
		return 0;

	return curl_easy_init();
}

void curl_test_clean(CURL* curl)
{
	if (curl)
	{
		curl_easy_cleanup(curl);
		curl_global_cleanup();
	}
}

CURLcode curl_test_perform(CURL* curl)
{
	return curl_easy_perform(curl);
}

CURLcode curl_test_post(CURL* curl, NET_OPT& no)
{
	return CURLE_OK;
}

CURLcode curl_test_httppost(CURL* curl, NET_OPT& nopt)
{
	return CURLE_OK;
}

CURLcode curl_test_put(CURL* curl, NET_OPT& nopt)
{
	return CURLE_OK;
}

size_t write_get_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	string str;
	str.append(ptr, size*nmemb);
	string strGBK = UTF8ToGBK(str);
	string* p = (string*)userdata;
	Json::Value root;
	Json::Reader reader;
	if (reader.parse(strGBK.c_str(), root))
	{
		*p = root["text"].asString();
	}
	return size*nmemb;
}

CURLcode curl_test_get(CURL* curl, NET_OPT& nopt)
{

	CURLcode cd = curl_easy_setopt(curl, CURLOPT_URL, nopt.strUrl.c_str());
	if (cd != CURLE_OK)
		return cd;

	cd = curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
	if (cd != CURLE_OK)
		return cd;

	cd = curl_easy_setopt(curl, CURLOPT_WRITEDATA, &nopt.text);
	if (cd != CURLE_OK)
		return cd;

	cd = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_get_callback);
	if (cd != CURLE_OK)
		return cd;

	return CURLE_OK;
}

bool curl_test_setopt(NET_OPT& nopt)
{
	CURLcode cd = CURLE_OK;
	CURL* curl = curl_test_init();
	if (curl == 0)
		return false;


	if (nopt.mode == NM_GET)
	{
		cd = curl_test_get(curl, nopt);
	}
	else if (nopt.mode == NM_HTTPPOST)
	{
		cd = curl_test_httppost(curl, nopt);
	}
	else if (nopt.mode == NM_POST)
	{
		cd = curl_test_post(curl, nopt);
	}
	else if (nopt.mode == NM_PUT)
	{
		cd = curl_test_put(curl, nopt);
	}

	if (cd != CURLE_OK)
		return false;

	cd = curl_test_perform(curl);
	if (cd != CURLE_OK)
		return false;

	curl_test_clean(curl);

	return true;
}
string GetResult(string input)
{
	NET_OPT netopt;
	string strurl = "http://www.tuling123.com/openapi/api?key=4c4cea3ea907488898463181e3a0fb95&info=";
	string inpututf8 = GBKToUTF8(input);
	netopt.strUrl = strurl + inpututf8;
	netopt.strUrl.append("&userid=[ÄãµÄid]");
	netopt.mode = NM_GET;
	if (curl_test_setopt(netopt))
	{
		return netopt.text;
	}
	return "";
}
