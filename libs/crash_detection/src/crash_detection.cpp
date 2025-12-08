#include "crash_detection/crash_detection.h"

#include <stacktrace>
#include <string>  // TODO TARVIIKO

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <cstdio>  // TODO TARVIIKO

LONG CALLBACK nativeCrashHandler(EXCEPTION_POINTERS* ep) {
    HANDLE hFile = CreateFileA("crash.log", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hFile != INVALID_HANDLE_VALUE) {
        // --- Write header ---
        DWORD       w;
        const char* header = "=== PROGRAM CRASHED ===\r\n";
        WriteFile(hFile, header, (DWORD)strlen(header), &w, nullptr);

        // --- Exception code ---
        DWORD code = ep->ExceptionRecord->ExceptionCode;

        char buffer[256];
        int  len = sprintf(buffer, "Exception code: 0x%08X\r\n", code);
        WriteFile(hFile, buffer, len, &w, nullptr);

        // --- Human readable reason ---
        const char* reason = "Unknown";
        switch (code) {
            case EXCEPTION_ACCESS_VIOLATION:
                reason = "Access Violation";
                break;
            case EXCEPTION_INT_DIVIDE_BY_ZERO:
                reason = "Divide by Zero";
                break;
            case EXCEPTION_INT_OVERFLOW:
                reason = "Integer Overflow";
                break;
            case EXCEPTION_ILLEGAL_INSTRUCTION:
                reason = "Illegal Instruction";
                break;
            case EXCEPTION_STACK_OVERFLOW:
                reason = "Stack Overflow";
                break;
            case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
                reason = "Array Bounds Exceeded";
                break;
            case EXCEPTION_FLT_DIVIDE_BY_ZERO:
                reason = "Float Divide by Zero";
                break;
            case EXCEPTION_FLT_OVERFLOW:
                reason = "Float Overflow";
                break;
            case EXCEPTION_BREAKPOINT:
                reason = "Breakpoint";
                break;
        }

        len = sprintf(buffer, "Reason: %s\r\n\r\n", reason);
        WriteFile(hFile, buffer, len, &w, nullptr);

        // TODO using c++23 backtrace could itself not be async-signal-safe since it could possibly allocate to heap??
        // If program crashed because of heap corruption this allocation can fail and the crash handler will itself
        // crash
        auto        st     = std::stacktrace::current();
        std::string st_str = to_string(st);

        WriteFile(hFile, st_str.c_str(), (DWORD)st_str.size(), &w, nullptr);

        CloseHandle(hFile);
    }

    return EXCEPTION_EXECUTE_HANDLER;
}
#endif

void installCrashDetector(const std::string_view file_name) {
    // Install native handler
#ifdef _WIN32
    AddVectoredExceptionHandler(1, nativeCrashHandler);
#endif
}