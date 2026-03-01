// SzxRunDll.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "RunDll.h"

using namespace FastRunDll;

int main()
{
    RunDll dll;
    auto msgbox_result =
        dll.CallDllFunc2_stdcall<int>("MessageBoxW", nullptr, L"消息文本", L"消息", MB_ICONWARNING | MB_YESNOCANCEL);
    std::printf("msgbox result %d\n", msgbox_result);
    system("pause");

    return 0;
}
