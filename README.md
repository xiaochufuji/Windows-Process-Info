# Encapsulation of Process and Window Information Retrieval on Windows
## Platform
This code runs on any platform that supports C++ 11 to C++ 20 standards. However, it has been tested on Windows 11, Visual Studio 2022 with MSVC C++ 11 to C++ 20.

## Feature Set
Encapsulates the retrieval of all running process information on Windows, including process IDs, process names, executable file paths of processes, etc.

Encapsulates the retrieval of all window information on Windows, including window handles, the process ID that owns the window, window visibility status, all child window handles, etc.

You can use the interface `std::vector<Intersection> getIntersection() const;` to obtain the windows of the running processes and perform other operations, such as checking the visibility of these windows and more.

The `Intersection `structure is a union of the above two pieces of information, meaning it contains all the details of both process and window information in one structure.

The code supports both wide and narrow character strings adaptively. It uses the PRINT macro for adaptive output, which works like std::cout or std::wcout.
## API Overview
```C++
// Get partial information about almost all running processes
std::vector<ProcessInformation::entry> getProcessInfos() const;
// Get partial information about almost all running Window
std::vector<ProcessInformation::handler> getWindowHandlers() const;
// Obtain the intersection of process information and window information. 
// The intersection is calculated based on the same process id. 
std::vector<Intersection> getIntersection() const;
```
```C++
ProcessInformation pe;
auto processes = pe.getProcessInfos();
auto windows = pe.getWindowHandlers();
for (const auto &ite : processes)
{
    PRINT << ite.th32ProcessID << "\t" << ite.szExeFile << std::endl;
}
for (const auto &ite : windows)
{
    PRINT << ite.hwnd << "\t" << ite.title << std::endl;
}
```
```C++
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
```
### For more details, please see the test case

# 封装Windows中进程信息获取以及窗口信息获取
## 平台
可在任意支持C++ 11 ~ 20标准中运行, 但是测试平台为Windows 11 , VS2022 MSVC C++ 11 ~ C++ 20
## 功能特性
封装了Windows中获取所有正在运行的进程信息，包括获取进程id, 进程名字，进程所郧西那个的exe文件路径等等...

封装了Windows中获取所有窗口信息，包括窗口句柄，窗口所属进程id，窗口是否可视化，以及窗口的所有子窗口的窗口句柄等等...

同时可以使用`std::vector<Intersection> getIntersection() const;`这个接口获取正在运行的进程的窗口, 进行其他的操作, 比如获取正在运行的进程窗口, 观察他的可视化等等等... `Intersection`就是上述两个信息的联合, 就是一个结构体里面包含了进程信息和窗口信息的所有信息

代码使用了宽字符串和窄字符自适应, 使用`PRINT`宏进行自适应, 可以看成std::cout或者std::wcout
## 接口介绍
```C++
// Get partial information about almost all running processes
std::vector<ProcessInformation::entry> getProcessInfos() const;
// Get partial information about almost all running Window
std::vector<ProcessInformation::handler> getWindowHandlers() const;
// Obtain the intersection of process information and window information. 
// The intersection is calculated based on the same process id. 
std::vector<Intersection> getIntersection() const;
```
```C++
ProcessInformation pe;
auto processes = pe.getProcessInfos();
auto windows = pe.getWindowHandlers();
for (const auto &ite : processes)
{
    PRINT << ite.th32ProcessID << "\t" << ite.szExeFile << std::endl;
}
for (const auto &ite : windows)
{
    PRINT << ite.hwnd << "\t" << ite.title << std::endl;
}
```
```C++
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
```
### 更详细的请看测试用例

