#ifndef LPAF_WIN32_H
#define LPAF_WIN32_H

#include <windows.h>
// dbghelp.h depends on symbols from windows.h
#include <dbghelp.h>

#ifdef PLATFORM_WINDOWS

#define FWI_HANDLE_FATAL_WIN32_ERROR fwiHandleWin32Error(__func__, __LINE__, GetLastError())

struct fwiWinAPIState {
    HINSTANCE instance;
    const wchar_t* className;
    DWORD nativeEventThreadHandle;
} __attribute__((aligned(32)));
struct fwiWinAPIState* fwiGetWinState();

LRESULT CALLBACK fwiWindowProc(
        HWND hwnd,
        UINT uMsg,
        WPARAM wParam,
        LPARAM lParam
        );

/**
 * @brief Event procedure that is executed in another thread; Calls event callbacks and such
 */
DWORD fwiEventProcedure(
        void* param
        );

/**
 * @brief Do not call, use @c FWI_HANDLE_FATAL_WIN32_ERROR macro instead
 */
void fwiHandleWin32Error(
        const char* sourceFunc,
        int line,
        int code
        );

#endif // PLATFORM_WINDOWS

#endif //LPAF_WIN32_H
