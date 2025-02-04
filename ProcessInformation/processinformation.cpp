#include "processinformation.h"
#include <iostream>
#include <memory>
#include <unordered_set>
#include <unordered_map>

ProcessInformation::ProcessInformation()
{
    m_processInfoVec.clear();
    _getProcessInfos();
    _getTopWindowHandlers();
}

ProcessInformation::~ProcessInformation()
{
}

std::vector<ProcessInformation::entry> ProcessInformation::getProcessInfos() const
{
    if (m_processInfoVec.empty())
        return std::vector<entry>();
    return m_processInfoVec;
}

std::vector<ProcessInformation::handler> ProcessInformation::getWindowHandlers() const
{
    if (m_processInfoVec.empty())
        return std::vector<ProcessInformation::handler>();
    return m_topWindowHandler;
}

std::vector<Intersection> ProcessInformation::getIntersection() const
{
    std::unordered_map<DWORD, _PROCESSENTRY32> processMap{};
    for (const auto& entry : m_processInfoVec) {
        processMap[entry.th32ProcessID] = entry;
    }

    std::vector<Intersection> result;
    for (const auto& handler : m_topWindowHandler) {
        auto it = processMap.find(handler.processId);
        if (it != processMap.end()) {
            result.push_back(std::move(Intersection{ handler, it->second }));
        }
    }
    return result;
}

void ProcessInformation::_getProcessInfos()
{
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return;
    }

    PROCESSENTRY32 pe{};
    pe.dwSize = sizeof(PROCESSENTRY32);
    if (Process32First(hSnapshot, &pe)) { 
        do {
            _PROCESSENTRY32 _pe(pe);
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pe.th32ProcessID);
            if (hProcess) {
                std::unique_ptr<_char[]> exePath = std::make_unique<_char[]>(MAX_PATH);
                DWORD dwSize = MAX_PATH;
                if (QueryFullProcessImageName(hProcess, 0, exePath.get(), &dwSize)) {
                    _pe.sExeFilePath = exePath.get();
                }
                FILETIME creationTime, exitTime, kernelTime, userTime;
                if (GetProcessTimes(hProcess, &creationTime, &exitTime, &kernelTime, &userTime)) {
                    FileTimeToSystemTime(&creationTime, &_pe.creationTime);

                    if (exitTime.dwLowDateTime != 0 || exitTime.dwHighDateTime != 0) {
                        FileTimeToSystemTime(&exitTime, &_pe.exitTime);
                    }
                    _pe.kernelTime.LowPart = kernelTime.dwLowDateTime;
                    _pe.kernelTime.HighPart = kernelTime.dwHighDateTime;
                    _pe.userTime.LowPart = userTime.dwLowDateTime;
                    _pe.userTime.HighPart = userTime.dwHighDateTime;
                }
                CloseHandle(hProcess); 
            }
            m_processInfoVec.push_back(_pe);
        } while (Process32Next(hSnapshot, &pe)); 
    }
    CloseHandle(hSnapshot); 
}

static DWORD CALLBACK GetProcessId_(HWND hwnd)
{
    DWORD processId;
    GetWindowThreadProcessId(hwnd, &processId);
    return processId;
}

static bool CALLBACK GetIsVisable(HWND hwnd)
{
    return IsWindowVisible(hwnd);
}

static _string CALLBACK GetWindowTitle(HWND hwnd) {
    const int len = GetWindowTextLength(hwnd) + 1;
    std::unique_ptr<_char[]> title = std::make_unique<_char[]>(len);
    if (!title) return _string{}; 
    GetWindowText(hwnd, title.get(), len);
    return _string(title.get());  
}

static _string CALLBACK GetWindowClass(HWND hwnd) {
    const int len = GetWindowTextLength(hwnd) + 1;
    std::unique_ptr<_char[]> className = std::make_unique<_char[]>(len);
    if (!className) return _string{};  
    GetClassName(hwnd, className.get(), len);
    return _string(className.get());  
}

static std::vector<HWND> CALLBACK GetChildWindows(HWND hwndParent) {
    std::vector<HWND> childHWNDs{};
    EnumChildWindows(hwndParent, 
        [](HWND hwndChild, LPARAM lParam) -> BOOL {
            std::vector<HWND>* childHWNDs = reinterpret_cast<std::vector<HWND>*>(lParam);
            int len = GetWindowTextLength(hwndChild) + 1;
            std::unique_ptr<_char[]> title = std::make_unique<_char[]>(len);
            GetWindowText(hwndChild, title.get(), len);
            childHWNDs->push_back(hwndChild);
            return TRUE;
        }, reinterpret_cast<LPARAM>(&childHWNDs));
    return childHWNDs;
}

void ProcessInformation::_getTopWindowHandlers()
{
    EnumWindows(
        [](HWND hwnd, LPARAM lParam) -> BOOL {
            std::vector<ProcessInformation::handler>* topWindowHandler = reinterpret_cast<std::vector<ProcessInformation::handler>*>(lParam);
            topWindowHandler->push_back(
                std::move(ProcessInformation::handler(
                    {
                        hwnd,
                        GetProcessId_(hwnd),
                        GetIsVisable(hwnd),
                        std::move(GetWindowTitle(hwnd)),
                        std::move(GetWindowClass(hwnd)),
                        std::move(GetChildWindows(hwnd))
                    }
                ))
            );
            return TRUE;
        }, reinterpret_cast<LPARAM>(&m_topWindowHandler));
}
