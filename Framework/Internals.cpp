// MIT License
//
// Copyright(c) 2023 ToneXum
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <algorithm>
#include <chrono>
#include <ctime>
#include <iostream>
#include <sstream>

#include "Internals.hpp"

void i::CreateManualError(int line, const char* func, const char* msg)
{
    std::ostringstream str;

    str << "An operation within the framework has caused an error:\n\n";
    str << msg << "\n\n";
    str << "Origin: " << func << " at " << line << "\n\n";
    str << "This is an internal error likely caused by the framework itself. ";
    str << "The program is unable to recover, the application must quit now!";
    str << std::flush;

    MessageBoxA(nullptr, str.str().c_str(), "Internal Error",
                uint64_t{MB_TASKMODAL} |
                uint64_t{MB_OK} |
                uint64_t{MB_ICONERROR});

    DeAlloc();
    std::exit(-1);
}

void i::DeAlloc()
{
    i::ProgramState* progState = i::GetState();

    progState->win32->nativeHandleMap.clear();
    progState->handleMap.clear();

    f::UnInitialise();

    if (progState->initialisationState & i::IfNetwork)
        f::UnInitialiseNetworking();
}

void i::DoNothingVv()
{ /* Well guess what, this function does nothing */ }

bool i::DoNothingBv()
{ return true; }

i::WindowData* i::GetWindowData(HWND handle)
{
    if (const auto found = i::GetState()->win32->nativeHandleMap.find(handle);
        found != i::GetState()->win32->nativeHandleMap.end())
    {
        return found->second.get();
    }

    return nullptr;
}

i::WindowData* i::GetWindowData(f::WndH handle)
{
    if (const auto found = i::GetState()->handleMap.find(handle);
        found != i::GetState()->handleMap.end())
    {
        return found->second.get();
    }

    return nullptr;
}

void i::EraseWindowData(HWND hWnd)
{
    std::unique_lock lock(i::GetState()->windowDataMutex);

    auto res = i::GetState()->win32->nativeHandleMap.find(hWnd); // find data to be erased

    std::wostringstream msg;
    msg << "Data for Window " << res->second->id << " was deleted";
    i::Log(msg.str().c_str(), i::LlDebug);

    i::GetState()->handleMap.erase(res->second->id); // erase data from the id map using the id
    i::GetState()->win32->nativeHandleMap.erase(res); // erase data from handle map using the iterator
}

void i::Log(const wchar_t* msg, LogLvl logLvl)
{
#ifdef _DEBUG
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);

    i::GetState()->loggerMutex.lock();
    std::cout << "[" << std::put_time(std::localtime(&currentTime), "%d.%m. %H:%M:%S") << "]";

    switch (logLvl)
    {
        case i::LlInfo:
        {
            std::cout << "[Info]: ";
            break;
        }
        case i::LlDebug:
        {
            std::cout << "[Debug]: ";
            break;
        }
        case i::LlWarning:
        {
            std::cout << "[Warning]: ";
            break;
        }
        case i::LlError:
        {
            std::cout << "[Error]: ";
            break;
        }
        case i::LlValidation:
        {
            std::cout << "[Valid]: ";
            break;
        }
        case LlBench:
        {
            std::cout << "[Bench]: ";
            break;
        }
    }

    std::wcout << msg << "\n" << std::flush;
    i::GetState()->loggerMutex.unlock();
#endif // _DEBUG
}

void i::Log(const char* msg, LogLvl logLvl)
{
#ifdef _DEBUG
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);

    i::GetState()->loggerMutex.lock();
    std::cout << "[" << std::put_time(std::localtime(&currentTime), "%d.%m. %H:%M:%S") << "]";

    switch (logLvl)
    {
        case i::LlInfo:
        {
            std::cout << "[Info]: ";
            break;
        }
        case i::LlDebug:
        {
            std::cout << "[Debug]: ";
            break;
        }
        case i::LlWarning:
        {
            std::cout << "[Warning]: ";
            break;
        }
        case i::LlError:
        {
            std::cout << "[Error]: ";
            break;
        }
        case i::LlValidation:
        {
            std::cout << "[Valid]: ";
        }
        case LlBench:
        {
            std::cout << "[Bench]: ";
            break;
        }
    }

    std::cout << msg << "\n" << std::flush;
    i::GetState()->loggerMutex.unlock();
#endif // _DEBUG
}

i::ProgramState* i::GetState()
{
    return i::programState.get();
}

i::NetworkState* i::GetNetworkState()
{
    return i::networkState.get();
}

i::ProgramState::ProgramState()
{

    win32 = std::make_unique<i::Win32State>();
    vulkan = std::make_unique<v::VulkanState>();
}

i::Socket::~Socket()
{
    FreeAddrInfoExW(nativeAddressInformation);
}
