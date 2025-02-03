#pragma once
#include "processinformation.h"
#include <iostream>

#if defined(UNICODE) || defined(_UNICODE) 
// use unicode set(unicode±àÂë)
#define PRINT std::wcout
#else
// use multi-bytes set(¶à×Ö½Ú±àÂë)
#define PRINT std::cout
#endif

void test()
{
    // set your locale
    std::wcout.imbue(std::locale("chs"));  // chinese
    ProcessInformation pe;
    auto intersection = pe.getIntersection();
    for (const auto& ite : intersection)
    {
        if (ite.handler.isVisable)
        {
            PRINT
                << "process_id: " << ite.entry.th32ProcessID
                << "\texe_file: " << ite.entry.szExeFile
                << "\tcreate_time: " << ite.entry.creationTime.wHour << ":" << ite.entry.creationTime.wMinute << ":" << ite.entry.creationTime.wSecond
                << "\twindow_title: " << ite.handler.title
                << "\thandler: " << ite.handler.hwnd;
            PRINT 
                << "\n";
            for (const auto& child : ite.handler.childHWNDs)
            {
                const int len = GetWindowTextLength(child) + 1;
                std::unique_ptr<_char[]> title = std::make_unique<_char[]>(len);
                GetWindowText(child, title.get(), len);
                PRINT << "\t" << "title: " << title.get() << " \thandler: " << child << std::endl;
            }
            PRINT << std::endl;
        }
    }
    PRINT << GetForegroundWindow() << std::endl;
}