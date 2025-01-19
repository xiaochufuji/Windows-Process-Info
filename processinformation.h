#ifndef __PROCESSINFORMATION_H
#define __PROCESSINFORMATION_H
#include <vector>
#include <Windows.h>
#include <tlhelp32.h>
#include <string>
#include <functional>

// right click your projet -> properties -> advance -> charater set
#ifdef UNICODE or _UNICODE
    // use unicode set(unicode±àÂë)
    #define _string std::wstring
    #define _char wchar_t
    #define _sizec (sizeof(wchar_t))
#else
    // use multi-bytes set(¶à×Ö½Ú±àÂë)
    #define _string std::string
    #define _char char
    #define _sizec (sizeof(char))
#endif


struct _WindowHanlerInfo;  
struct _PROCESSENTRY32;    

struct _PROCESSENTRY32 {
    _PROCESSENTRY32() = default;
    _PROCESSENTRY32(const _PROCESSENTRY32&) = default;
    _PROCESSENTRY32& operator=(const _PROCESSENTRY32&) = default;
    _PROCESSENTRY32(const PROCESSENTRY32& pe) :
        dwSize{ pe.dwSize }, cntUsage{ pe.cntUsage }, th32ProcessID{ pe.th32ProcessID },
        th32DefaultHeapID{ pe.th32DefaultHeapID }, th32ModuleID{ pe.th32ModuleID }, cntThreads{ pe.cntThreads },
        th32ParentProcessID{ pe.th32ParentProcessID }, pcPriClassBase{ pe.pcPriClassBase }, dwFlags{ pe.dwFlags },
        szExeFile{ pe.szExeFile }
    {
    }

    DWORD dwSize{ 0 };
    DWORD cntUsage{ 0 };
    DWORD th32ProcessID{ 0 };
    ULONG_PTR th32DefaultHeapID{ NULL };
    DWORD th32ModuleID{ 0 };
    DWORD cntThreads{ 0 };
    DWORD th32ParentProcessID{ 0 };
    LONG pcPriClassBase{ 0 };
    DWORD dwFlags{ 0 };
    _string szExeFile{};

    // Additional members
    _string sExeFilePath{};
    SYSTEMTIME creationTime{ 0, 0, 0, 0, 0, 0, 0, 0 };
    SYSTEMTIME exitTime{ 0, 0, 0, 0, 0, 0, 0, 0 };
    ULARGE_INTEGER kernelTime{};
    ULARGE_INTEGER userTime{};
};

struct _WindowHanlerInfo {
    _WindowHanlerInfo() = default;
    _WindowHanlerInfo(const _WindowHanlerInfo&) = default;
    _WindowHanlerInfo& operator=(const _WindowHanlerInfo&) = default;
    HWND hwnd{ 0 };
    DWORD processId{ 0 };
    bool isVisable{ false };
    _string title{};
    _string class_{};
    std::vector<HWND> childHWNDs{};
};

struct Intersection
{
    Intersection() = delete;
    Intersection(const Intersection&) = default;
    Intersection& operator=(const Intersection&) = default;
    Intersection(const _WindowHanlerInfo& handler, const _PROCESSENTRY32& entry)
        : entry(entry), handler(handler) {
    }
    const _PROCESSENTRY32 entry;
    const _WindowHanlerInfo handler;
};

class ProcessInformation
{
	using entry = _PROCESSENTRY32;
    using handler = _WindowHanlerInfo;
public:
	ProcessInformation();
	~ProcessInformation();

    // Get partial information about almost all running processes
    std::vector<ProcessInformation::entry> getProcessInfos() const;
    // Get partial information about almost all running Window
    std::vector<ProcessInformation::handler> getWindowHandlers() const;
    // Obtain the intersection of process information and window information. 
    // The intersection is calculated based on the same process id. 
    std::vector<Intersection> getIntersection() const;

private:
	void _getProcessInfos();
    void _getTopWindowHandlers();

private:
	std::vector<ProcessInformation::entry> m_processInfoVec;
    std::vector<ProcessInformation::handler> m_topWindowHandler;
};


#endif


