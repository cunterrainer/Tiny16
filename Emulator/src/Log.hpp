#ifndef LOG_HPP
#define LOG_HPP
#include <format>
#include <string>
#include <cerrno>
#include <cstring>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <string_view>
#include <source_location>

// ERR acts as an assert calling std::abort when used
#ifdef BUILD_DEBUG
    #define LOG(fmt, ...) Log::Impl::Log(fmt, __VA_ARGS__)
    #define ERR(fmt, ...) Log::Impl::Err(std::source_location::current(), fmt, __VA_ARGS__)
    #define LOG_REASON(fmt, ...) Log::Impl::LogReason(fmt, __VA_ARGS__)
    #define ERR_REASON(fmt, ...) Log::Impl::ErrReason(std::source_location::current(), fmt, __VA_ARGS__)

    #define LOG_IF(cond, fmt, ...) if (cond) { LOG(fmt, __VA_ARGS__); }
    #define ERR_IF(cond, fmt, ...) if (cond) { ERR(fmt, __VA_ARGS__); }
    #define LOG_REASON_IF(cond, fmt, ...) if (cond) { LOG_REASON(fmt, __VA_ARGS__); }
    #define ERR_REASON_IF(cond, fmt, ...) if (cond) { ERR_REASON(fmt, __VA_ARGS__); }
#elif defined NDEBUG
    #define LOG(fmt, ...) Log::Impl::Log(fmt, __VA_ARGS__)
    #define ERR(fmt, ...)
    #define LOG_REASON(fmt, ...) Log::Impl::LogReason(fmt, __VA_ARGS__)
    #define ERR_REASON(fmt, ...)

    #define LOG_IF(cond, fmt, ...) if (cond) { LOG(fmt, __VA_ARGS__); }
    #define ERR_IF(cond, fmt, ...)
    #define LOG_REASON_IF(cond, fmt, ...) if (cond) { LOG_REASON(fmt, __VA_ARGS__); }
    #define ERR_REASON_IF(cond, fmt, ...)
#endif


namespace Log::Impl
{
    template <typename... Args>
    inline void Log(std::format_string<Args...> fmt, Args&&... args)
    {
        std::cout << "[Emulator] " << std::format(fmt, std::forward<Args>(args)...) << std::endl;
    }


    template <typename... Args>
    inline void LogReason(std::format_string<Args...> fmt, Args&&... args)
    {
        std::cout << "[Emulator] " << std::format(fmt, std::forward<Args>(args)...) << ", Reason: " << strerror(errno) << std::endl;
    }
}


#ifdef BUILD_DEBUG
#ifdef PLATFORM_WINDOWS
    #include <Windows.h>
    #undef min
    #undef max

    namespace Log::Impl::Win
    {
        inline std::string GetLastErrorAsString()
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


        inline bool EnableAnsiEscapeSequences()
        {
            const HANDLE handle = GetStdHandle(STD_ERROR_HANDLE);
            if (handle == INVALID_HANDLE_VALUE)
            {
                std::cerr << "[Emulator Win Error] Failed to get stdout handle: " << GetLastErrorAsString() << std::endl;
                return false;
            }

            // Check if the handle is a console handle
            if (GetFileType(handle) != FILE_TYPE_CHAR)
            {
                return false; // not a console
            }

            // Get the current console mode
            DWORD dwMode = 0;
            if (!GetConsoleMode(handle, &dwMode))
            {
                std::cerr << "[Emulator Win Error] Failed to get console mode: " << GetLastErrorAsString() << std::endl;
                return false;
            }

            // Enable Virtual Terminal Processing
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            if (!SetConsoleMode(handle, dwMode))
            {
                std::cerr << "[Emulator Win Error] Failed to set console mode: " << GetLastErrorAsString() << std::endl;
                return false;
            }
            return true;
        }
    }
#elif defined PLATFORM_UNIX
    #include <unistd.h>

    namespace Log::Impl::Unix
    {
        inline bool IsTerminal() noexcept
        {
            return isatty(STDOUT_FILENO); // We have to trust that the terminal support ansi escape codes
        }
    }
#endif // PLATFORM_UNIX


namespace Log::Impl
{
    namespace Global
    {
        // Has to be global because if it's in Err it will be called
        // for every Err overload which can be a lot since it's a templated function
        inline const bool AnsiEnabled = []()
        {
            #ifdef PLATFORM_WINDOWS
                return Log::Impl::Win::EnableAnsiEscapeSequences();
            #elif defined PLATFORM_UNIX
                return Log::Impl::Unix::IsTerminal();
            #else
                return false;
            #endif
        }();

        inline const std::string_view RedSequence = AnsiEnabled ? "\033[31m" : "";
        inline const std::string_view ResetSequence = AnsiEnabled ? "\033[0m" : "";
    }


    template <typename... Args>
    inline void Err(const std::source_location& location, std::format_string<Args...> fmt, Args&&... args)
    {
        std::cerr << Log::Impl::Global::RedSequence << "[Emulator Error] " << std::format(fmt, std::forward<Args>(args)...) << " in file " << location.file_name() << ", line " << location.line() << Log::Impl::Global::ResetSequence << std::endl;
        std::abort();
    }


    template <typename... Args>
    inline void ErrReason(const std::source_location& location, std::format_string<Args...> fmt, Args&&... args)
    {
        std::cerr << Log::Impl::Global::RedSequence << "[Emulator Error] " << std::format(fmt, std::forward<Args>(args)...) << " in file " << location.file_name() << ", line " << location.line() << ", Reason: " << strerror(errno) << Log::Impl::Global::ResetSequence << std::endl;
        std::abort();
    }
}
#endif //BUILD_DEBUG
#endif // LOG_HPP
