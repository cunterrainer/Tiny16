#ifndef LOG_H
#define LOG_H
#include <format>
#include <string>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <string_view>
#include <source_location>

// ERR acts as an assert calling std::abort when used
#ifdef BUILD_DEBUG
    #define LOG(fmt, ...) Log::Impl::Log(fmt, __VA_ARGS__)
    #define ERR(fmt, ...) Log::Impl::Err(std::source_location::current(), fmt, __VA_ARGS__)

    #define LOG_IF(cond, fmt, ...) if (cond) { LOG(fmt, __VA_ARGS__); }
    #define ERR_IF(cond, fmt, ...) if (cond) { ERR(fmt, __VA_ARGS__); }
#elif defined NDEBUG
    #define LOG(fmt, ...)
    #define ERR(fmt, ...)

    #define LOG_IF(cond, fmt, ...)
    #define ERR_IF(cond, fmt, ...)
#endif

#ifdef BUILD_DEBUG
#ifdef PLATFORM_WINDOWS
#include <Windows.h>

namespace Log::Impl::Win
{
    std::string WinGetLastErrorAsString()
    {
        const DWORD errorMessageID = ::GetLastError();
        if (errorMessageID == 0)
        {
            return std::string(); //No error message has been recorded
        }

        LPSTR messageBuffer = nullptr;
        //Ask Win32 to give us the string version of that message ID.
        //The parameters we pass in, tell Win32 to create the buffer that holds the message for us (because we don't yet know how long the message string will be).
        const DWORD size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);
        const std::string message(messageBuffer, static_cast<std::size_t>(size));
        LocalFree(messageBuffer);
        return message;
    }


    bool WinEnableAnsiEscapeSequences()
    {
        const HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
        if (handle == INVALID_HANDLE_VALUE)
        {
            std::cerr << "[Emulator Win Error] Failed to get stdout handle: " << WinGetLastErrorAsString() << std::endl;
            return false;
        }

        // Get the current console mode
        DWORD dwMode = 0;
        if (!GetConsoleMode(handle, &dwMode))
        {
            std::cerr << "[Emulator Win Error] Failed to get console mode: " << WinGetLastErrorAsString() << std::endl;
            return false;
        }

        // Enable Virtual Terminal Processing
        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        if (!SetConsoleMode(handle, dwMode))
        {
            std::cerr << "[Emulator Win Error] Failed to set console mode: " << WinGetLastErrorAsString() << std::endl;
            return false;
        }
        return true;
    }
}
#endif // PLATFORM_WINDOWS


namespace Log::Impl
{
    template <typename... Args>
    inline void Log(std::format_string<Args...> fmt, Args&&... args)
    {
        std::cout << "[Emulator Log] " << std::format(fmt, std::forward<Args>(args)...) << std::endl;
    }


    namespace Global
    {
        // Has to be global because if it's in ImplErr it will be called
        // for every ImplErr overload which can be a lot because it's a templated function
        static const bool AnsiEnabled = []()
        {
            #ifdef PLATFORM_WINDOWS
                return Log::Impl::Win::WinEnableAnsiEscapeSequences();
            #else
                return false; // TODO: Add Linux & macos support
            #endif
        }();

        static const std::string_view RedSequence = AnsiEnabled ? "\033[31m" : "";
        static const std::string_view ResetSequence = AnsiEnabled ? "\033[0m" : "";
    }


    template <typename... Args>
    inline void Err(const std::source_location& location, std::format_string<Args...> fmt, Args&&... args)
    {
        std::cerr << Log::Impl::Global::RedSequence << "[Emulator Error] " << std::format(fmt, std::forward<Args>(args)...) << " in file " << location.file_name() << ", line " << location.line() << ", function " << location.function_name() << Log::Impl::Global::ResetSequence << std::endl;
        std::abort();
    }
}
#endif //BUILD_DEBUG
#endif // LOG_H