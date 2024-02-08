#include "pch.h"
#include <windows.h>
#include <iostream>
#include <Tlhelp32.h>
#include <stdio.h>
#include <tchar.h>
#include <string>
#include <Shlwapi.h> // 需要包含这个头文件以使用PathCombine 
using namespace std;


/// <summary>
/// 根据进程名称获取进程信息
/// </summary>
/// <param name="info"></param>
/// <param name="processName"></param>
/// <returns></returns>
BOOL getProcess32Info(PROCESSENTRY32* info, const TCHAR processName[])
{
    HANDLE handle; //定义CreateToolhelp32Snapshot系统快照句柄
    handle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);//获得系统快照句柄
    //PROCESSENTRY32 结构的 dwSize 成员设置成 sizeof(PROCESSENTRY32)
    info->dwSize = sizeof(PROCESSENTRY32);
    //调用一次 Process32First 函数，从快照中获取进程列表
    Process32First(handle, info);
    //重复调用 Process32Next，直到函数返回 FALSE 为止

    while (Process32Next(handle, info) != FALSE)
    {
        if (wcscmp(processName, info->szExeFile) == 0)
        {
            return TRUE;
        }
    }
    return FALSE;
}

/// <summary>
/// 注入DLL文件
/// </summary>
/// <param name="DllFullPath">DLL文件的全路径</param>
/// <param name="dwRemoteProcessId">要注入的程序的PID</param>
/// <returns></returns>
int InjectDLL(const wchar_t* DllFullPath, const DWORD pid)
{

    HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, false, pid);//权限 不继承此句柄 要打开的目标-----返回指定进程的打开句柄
    if (hProc == 0) return -1;

    // 计算路径的字节数
    int pathSize = (wcslen(DllFullPath) + 1) * sizeof(wchar_t);
    WCHAR str[10];
    _itow_s(pathSize, str, 10, 10);

    //指定进程的虚拟地址空间中保留或开辟一段区域（初始化内存）
    //无类型指针 LPVOID
    //申请内存所在的进程句柄
    // NULL自动分配
    //欲分配的内存大小
    LPVOID buffer = VirtualAllocEx(hProc, 0, pathSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (buffer == 0) return -2;

    //把自定义的dll文件注入目标进程，并判断是否写入成功
    if (!WriteProcessMemory(hProc, buffer, DllFullPath, pathSize, NULL)) return -3;

    //调用Kernel32.dll中的LoadLibraryW方法用以加载DLL文件
    LPVOID pFunc = GetProcAddress(GetModuleHandleA("Kernel32.dll"), "LoadLibraryW");

    //创建一个在另一个进程的虚拟地址空间中运行的线程
    CreateRemoteThread(hProc, 0, 0, (LPTHREAD_START_ROUTINE)pFunc, buffer, 0, 0);
    //MessageBox(NULL, str, L"pathSize注入成功", MB_OK);
}


wstring GetProgramDir()
{
    TCHAR exeFullPath[MAX_PATH]; // Full path 
    GetModuleFileName(NULL, exeFullPath, MAX_PATH);
    wstring strPath = __TEXT("");
    strPath = (wstring)exeFullPath;    // Get full path of the file 
    int pos = strPath.find_last_of(L'\\', strPath.length());
    return strPath.substr(0, pos);  // Return the directory without the file name 
}

int main(int argc, char* argv[])
{

    PROCESSENTRY32 info;//TIHelp32.h
    if (getProcess32Info(&info, L"WeChat.exe"))
    {
        wstring fullPath = GetProgramDir();
        fullPath.append(L"\\getMsgDllDemo.dll");
        //TCHAR str[0x20];
        //memset(str, 0, 0x20);
       /* wsprintf(str, TEXT("%d"), info.th32ProcessID);
        MessageBox(NULL, fullPath.c_str(), L"测试", MB_OK);
        MessageBox(NULL, str, L"测试1", MB_OK);*/
        InjectDLL(fullPath.c_str(), info.th32ProcessID);
    }
    else {
        MessageBox(NULL, L"查找失败", L"测试2", MB_OK);
    }
    return 0;
}
