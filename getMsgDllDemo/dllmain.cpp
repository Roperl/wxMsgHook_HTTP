#include "pch.h"
#include<Windows.h>
#include<string>
#include<stdio.h>
#include <iostream>
#include <fstream>
#include<istream>
#include "pch.h"
#include<iostream>
#include <locale>  
#include <codecvt>  
#include<WinSock2.h>
#pragma comment(lib,"ws2_32.lib")

using namespace std;
#define HOOK_LEN 5
#define HOOK_ADD 0xCED0D7
#define RET_ADD 0xCED0DC
#define SOURCE_CALL_ADD 0xCF20E0
#define WXMSGID_OFFSET 0x188//消息唯一id
#define WXID_OFFSET 0x48 //收到消息的群或发送人的id
#define WXQ_SENDER_OFFSET 0x174//群中发送人id，如果是自己就没有，如果是别人就有消息
#define MSG_OFFSET 0x70//消息内容
#define TXT_PATH "F:\\桌面\\try.txt "

BYTE backCode[HOOK_LEN] = { 0 };
//获取基址
DWORD getWechatWin() {
	return (DWORD)LoadLibrary(L"WeChatWin.dll");
}

int httpGet(const char* bufSend)
{

	//开始进行socket初始化
	WSADATA wData;
	::WSAStartup(MAKEWORD(2, 2), &wData);

	SOCKET clientSocket = socket(AF_INET, 1, 0);
	struct sockaddr_in ServerAddr = { 0 };
	int Ret = 0;
	int AddrLen = 0;
	HANDLE hThread = 0;
	ServerAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	ServerAddr.sin_port = htons(8888);;
	ServerAddr.sin_family = AF_INET;
	char bufRecv[3069] = { 0 };
	int errNo = 0;
	errNo = connect(clientSocket, (sockaddr*)&ServerAddr, sizeof(ServerAddr));
	if (errNo == 0)
	{
		send(clientSocket, bufSend, strlen(bufSend), 0);
		::WSACleanup();

	}
	else
	{
		::WSACleanup();
	}
	//socket环境清理

	return 0;
}


//这个函数是将宽字符的微信内容存到txt文件中去的转化函数
void saveWideFile(ostream& out, wchar_t const* str, int size)
{

		char const* pos = (char   const*)str;
		if (str[0] != 0xFEFF) // 写入unicode文件头
		{
			char const* const utf16head = "\xFF\xFE ";
			out.write(utf16head, 2);
		}

		out.write(pos, size);

}
VOID saveMsg(DWORD msgcode) {

		DWORD wxidadd = msgcode + WXID_OFFSET;
		DWORD msgadd = msgcode + MSG_OFFSET;
		DWORD msgMsgId = msgcode + WXMSGID_OFFSET;
		DWORD msgWxqSender = msgcode + WXQ_SENDER_OFFSET;


		//wchar_t Wxid[0xFF] = { 0 };
		//wchar_t Msg[0xFFF] = { 0 };
		//wchar_t WXMsgId[0xFF] = { 0 };
		//wchar_t WXWxqSender[0xFF] = { 0 };
		wchar_t url[0xFFFF] = { 0 };
		swprintf_s(url, L"﻿GET /recvmsg?msgId=%s&msgForm=%s&msg=%s&wxqunSender=%s  HTTP/1.1 \r\n"
			"Accept : */*"
			, (wchar_t*)*((LPVOID*)msgMsgId), (wchar_t*)*((LPVOID*)wxidadd), (wchar_t*)*((LPVOID*)msgadd), (wchar_t*)*((LPVOID*)msgWxqSender));
		/*swprintf_s(Wxid, L"&msgForm=%s", (wchar_t*)*((LPVOID*)wxidadd));
		swprintf_s(Msg, L"&msg=%s", (wchar_t*)*((LPVOID*)msgadd));
		swprintf_s(WXMsgId, L"msgId=%s", (wchar_t*)*((LPVOID*)msgMsgId));
		swprintf_s(WXWxqSender, L"&wxqunSender=%s", (wchar_t*)*((LPVOID*)msgWxqSender));*/
		//MessageBox(NULL, url, L"测试", MB_OK);
		std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
		std::string str = converter.to_bytes(url);
		httpGet(str.c_str());
		memset(url, 0, sizeof(url));


		/*int MsgLen = wcslen(Msg) * 2 ;
		int WxidLen = wcslen(Wxid) * 2 ;
		int WXMsgIdLen = wcslen(WXMsgId) * 2 ;
		int WXWxqSenderLen = wcslen(WXWxqSender) * 2 ;

		ofstream out(TXT_PATH, ios::binary | ios::out | ios::app);
		saveWideFile(out, WXMsgId, WXMsgIdLen);
		saveWideFile(out, Wxid, WxidLen);
		saveWideFile(out, Msg, MsgLen);
		saveWideFile(out, WXWxqSender, WXWxqSenderLen);
		out.close();*/
	
}




DWORD pEax = 0;
DWORD pEcx = 0;
DWORD pEdx = 0;
DWORD pEbx = 0;
DWORD pEsp = 0;
DWORD pEbp = 0;
DWORD pEsi = 0;
DWORD pEdi = 0;
DWORD pEip = 0;

DWORD WinAdd = getWechatWin();
DWORD retAdd = WinAdd + RET_ADD;
DWORD SourceAdd = WinAdd + SOURCE_CALL_ADD;
//自己函数用来写入微信的
//声明一个裸函数（就是告诉编译器不做任何多余的事情操作）
VOID __declspec(naked)  showMsg() {

	//备份寄存器
	__asm {
		mov pEax, eax
		mov pEcx, ecx
		mov pEdx, edx
		mov pEbx, ebx
		mov pEsp, esp
		mov pEbp, ebp
		mov pEsi, esi
		mov pEdi, edi

	}
	//我们的二维码数据在ecx里，所以要写一个函数来保存二维码数据
	saveMsg(pEdi);

	//把寄存器恢复
	__asm {
		mov eax, pEax
		mov ecx, pEcx
		mov edx, pEdx
		mov ebx, pEbx
		mov esp, pEsp
		mov ebp, pEbp
		mov esi, pEsi
		mov edi, pEdi
		call SourceAdd
		jmp retAdd
	}

}





//开始hook
VOID startHook(LPVOID funAdd) {
	DWORD WinAdd = getWechatWin();
	//HOOK的地址
	DWORD hookAdd = WinAdd + HOOK_ADD;
	//组装数据byte
	BYTE jmpCode[HOOK_LEN] = { 0 };
	jmpCode[0] = 0xE9;
	//要跳转的地址-现在要hook的地址-5
	*(DWORD*)&jmpCode[1] = (DWORD)funAdd - hookAdd - 5;


	//获取自己的进程句柄
	HANDLE hWHND = OpenProcess(PROCESS_ALL_ACCESS, NULL, GetCurrentProcessId());
	//先记下它原本的内容，到时取消hook的时候就写回去
	if (ReadProcessMemory(hWHND, (LPCVOID)hookAdd, backCode, HOOK_LEN, NULL) == 0) {
		MessageBox(NULL, L"读取内存数据失败", L"错误", 0);
		return;
	}
	//写入我们组好的数据
	WriteProcessMemory(hWHND, (LPVOID)hookAdd, jmpCode, HOOK_LEN, NULL);


}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		startHook(showMsg);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

