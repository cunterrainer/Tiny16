/*
    Hint: User format string from C++20 std::format if you're on C++20 or above
    Hint: All the functions have a Take variant e.g. OkTake() returning a movable T&&

    Example:
        Result<std::string> ReadFile(const char* p)
        {
            // open file
            if (file_is_open == false)
                return Err("Failed to open file: %s", p);
            // read content
            return content;
            // return Ok(content); // alternative, if not implicitly convertible Ok<std::string>(content)
        }

        Result<std::string> r = ReadFile("test.txt");
        if (r) // if valid, can also use r.IsOk() or r.IsErr()
        {
            const std::string& str = r.Ok();
        }
        else // warning r.Ok() or r.Err() are not allowed to be used if valid or invalid respectively
        {
            std::cout << r.Err().What() << std::endl;
        }
    Void result:
        The only difference when using a Result<void> is that you have to explicitly return Result<void> e.g.:
        Result<void> ProcessFile(const char* p)
        {
            // open file
            if (file_is_open == false)
                return Err("Failed to open file: %s", p);
            // process
            return Ok(); // or return {}; or return Result<void>();
        }
    Error handling:
        Result<std::string> r = ReadFile("test.txt");
        r.Unwrap(); // If valid returns const T&, otherwise throws Error with generic message
        r.Expect(); // If valid returns const T&, otherwise throws Error with custom message
        r.UnwrapOr(""); // If valid returns const T&, otherwise parameter
        r.UnwrapOrDefault(); // If valid returns const T&, otherwise T()
        r.UnwrapOrElse(RandomString, 0, 10); // If valid returns const T&, otherwise function return value

        Unwrap() and Expect() can throw to catch:
            try
            {
                r.Unwrap();
                r.Expect("Failed to read file: ");
            }
            catch (const Err& e)
            {
                std::cout << e.What() << std::endl;
            }
    Error types:
        If using the default provided Err, you can add enums or numbers when construction Err
        Example:
            enum Errors
            {
                IOError, MemoryError
            };

            Result<std::string, Errors> ReadFile(const char* p)
            {
                // open file
                if (file_is_open == false)
                    return Error<Errors>(IOError, "Failed to open file: %s", p);
                // read content
                return content;
            }

            Result<std::string> r = ReadFile("test.txt");
            if (r.Err().Type() == IOError)
            {
                std::cout << r.Err().What() << std::endl;
            }
    Custom error types:
        To use an enum class define your own error type:
        enum class Errors
        {
            IOError, MemoryError
        };
        using OwnErr = Error<Errors>;

        Result<std::string, OwnErr> ReadFile(const char* p)
        {
            // open file
            if (file_is_open == false)
                return OwnErr(Errors::IOError, "Failed to open file: %s", p);
            // read content
            return content;
        }

        Result<std::string, OwnErr> r = ReadFile("test.txt");
        if (r.Err().Type() == Errors::IOError)
        {
            std::cout << r.Err().What() << std::endl;
        }
    Custom error structs:
        The easiest solution is to inherit from Err.
        Example:
        struct IOError : Err
        {
            using Err::Error;
        };

        Result<std::string, IOError> str = ReadFile("test.txt");

        If you don't want to inherit from Err, the error struct has to provide:
        struct IOError
        {
            // has to be copiable and movable
            inline IOError(const IOError& other);
            inline IOError(IOError&& other) noexcept;
            IOError& operator=(const IOError& other);
            IOError& operator=(IOError&& other) noexcept;
        };
        Result<type, IOError>::Unwrap() and Result<type, IOError>::Expect() will throw IOError
*/
#ifndef RESULT_H
#define RESULT_H
#include <cassert>
#include <limits>
#include <string>
#include <cstdint>
#include <utility>

#if __cplusplus >= 202002L
    #include <format>
    #define HAS_STD_FORMAT 1
#else
    #include <cstdio>
    #define HAS_STD_FORMAT 0
#endif


// Declare the template first otherwise compiler will complain
template <typename ErrorType>
class Error;

template <>
class Error<void>
{
private:
    mutable std::string m_What;
public:
    #if HAS_STD_FORMAT
        // Compile-time format string overload (safe, fast)
        template <typename... Args>
        inline explicit Error(std::format_string<Args...> what, Args&&... args) : m_What(std::format(what, std::forward<Args>(args)...)) {}
    #else
        template <typename... Args>
        explicit Error(const char* what, Args&&... args)
        {
            const int size = std::snprintf(NULL, 0, what, std::forward<Args>(args)...) + 1; // Extra space for '\0'
            m_What.resize(size);
            std::snprintf(m_What.data(), size, what, std::forward<Args>(args)...);
            m_What.resize(size - 1); // remove the '\0' terminator
        }
    #endif
    inline explicit Error(const char* what) : m_What(what) {}
    inline explicit Error(const std::string& what) : m_What(what) {}

    inline explicit Error() = default;

    inline Error(const Error& other) : m_What(other.m_What) {}
    inline Error(Error&& other) noexcept : m_What(std::move(other.m_What)) {}

    Error& operator=(const Error& other)
    {
        if (this != &other)
        {
            m_What = other.m_What;
        }
        return *this;
    }

    Error& operator=(Error&& other) noexcept
    {
        if (this != &other)
        {
            m_What = std::move(other.m_What);
        }
        return *this;
    }

    inline const std::string& What() const noexcept
    {
        return m_What;
    }

    inline std::string&& WhatTake() const noexcept
    {
        return std::move(m_What);
    }
};


template <typename ErrorType>
class Error : Error<void>
{
private:
    ErrorType m_Type;
public:
    #if HAS_STD_FORMAT
        // Error with type + format string (compile-time)
        template <typename... Args>
        inline explicit Error(ErrorType type, std::format_string<Args...> what, Args&&... args) : Error<void>(std::format(what, std::forward<Args>(args)...)), m_Type(type) {}
    #else
        template <typename... Args>
        inline explicit Error(ErrorType type, const char* what, Args&&... args) : Error<void>(what, std::forward<Args>(args)...), m_Type(type) {}
    #endif

    inline explicit Error(ErrorType type, const char* what) : Error<void>(what), m_Type(type) {}
    inline explicit Error(ErrorType type, const std::string& what) : Error<void>(what), m_Type(type) {}

    Error() = default;

    Error(const Error& other) : Error<void>(other), m_Type(other.m_Type) {}
    Error(Error&& other) noexcept : Error<void>(std::move(other)), m_Type(std::move(other.m_Type)) {}

    Error& operator=(const Error& other)
    {
        if (this != &other)
        {
            Error<void>::operator=(other);
            m_Type = other.m_Type;
        }
        return *this;
    }

    Error& operator=(Error&& other) noexcept
    {
        if (this != &other)
        {
            Error<void>::operator=(std::move(other));
            m_Type = std::move(other.m_Type);
        }
        return *this;
    }

    inline ErrorType Type() const noexcept
    {
        return m_Type;
    }
};
using Err = Error<void>;


namespace ResultUtil
{
    template <class... Types> // std::void_t from C++17
    using VoidT = void;
}

template <typename T, typename E = Err>
class Result;

template <typename T, typename E>
class Result
{
    static_assert(std::is_copy_assignable<E>::value, "Result::Error has to be copy assignable");
    static_assert(std::is_move_assignable<E>::value, "Result::Error type has to be move assignable");
    static_assert(std::is_copy_constructible<E>::value, "Result::Error type has to be copiable");
    static_assert(std::is_move_constructible<E>::value, "Result::Error type has to be movable");

    static_assert(std::is_destructible<T>::value, "Result<T, E> requires T to be destructible");
    static_assert(std::is_destructible<E>::value, "Result<T, E> requires E to be destructible");
private:
    union
    {
        mutable T m_Data;
        mutable E m_Error;
    };
    bool m_Valid;
public:
    inline Result(const E& e) : m_Valid(false)
    {
        new (&m_Error) E(e);
    }

    inline Result(E&& e) noexcept(std::is_nothrow_move_constructible<E>::value)
        : m_Valid(false)
    {
        new (&m_Error) E(std::move(e));
    }

    inline Result(const T& t) : m_Valid(true)
    {
        new (&m_Data) T(t);
    }

    inline Result(T&& t) noexcept(std::is_nothrow_move_constructible<T>::value)
        : m_Valid(true)
    {
        new (&m_Data) T(std::move(t));
    }

    Result(const Result& other) : m_Valid(other.m_Valid)
    {
        if (m_Valid)
            new (&m_Data) T(other.m_Data);
        else
            new (&m_Error) E(other.m_Error);
    }

    Result(Result&& other) noexcept(std::is_nothrow_move_constructible<T>::value && std::is_nothrow_move_constructible<E>::value) : m_Valid(other.m_Valid)
    {
        if (m_Valid)
            new (&m_Data) T(std::move(other.m_Data));
        else
            new (&m_Error) E(std::move(other.m_Error));
    }

    Result& operator=(const Result& other)
    {
        if (this != &other)
        {
            if (m_Valid && other.m_Valid)
            {
                m_Data = other.m_Data;  // both valid → safe assignment
            }
            else if (!m_Valid && !other.m_Valid)
            {
                m_Error = other.m_Error; // both invalid → safe assignment
            }
            else
            {
                // Different state → need to destroy the old and construct the new
                if (m_Valid)
                {
                    m_Data.~T();
                    new (&m_Error) E(other.m_Error);
                }
                else
                {
                    m_Error.~E();
                    new (&m_Data) T(other.m_Data);
                }
                m_Valid = other.m_Valid;
            }
        }
        return *this;
    }

    Result& operator=(Result&& other) noexcept(std::is_nothrow_move_assignable<T>::value && std::is_nothrow_move_assignable<E>::value)
    {
        if (this != &other)
        {
            if (m_Valid)
            {
                if (other.m_Valid)
                    m_Data = std::move(other.m_Data);
                else
                {
                    m_Data.~T();
                    new (&m_Error) E(std::move(other.m_Error));
                    m_Valid = false;
                }
            }
            else
            {
                if (other.m_Valid)
                {
                    m_Error.~E();
                    new (&m_Data) T(std::move(other.m_Data));
                    m_Valid = true;
                }
                else
                    m_Error = std::move(other.m_Error);
            }
        }
        return *this;
    }

    ~Result() noexcept(std::is_nothrow_destructible<T>::value && std::is_nothrow_destructible<E>::value)
    {
        if (m_Valid)
            m_Data.~T();
        else
            m_Error.~E();
    }

    inline const T& Ok() const noexcept
    {
        assert(m_Valid && "Don't access the Ok() value if it is an error, use IsOk() to check beforehand!");
        return m_Data;
    }

    inline T&& OkTake() const noexcept(std::is_nothrow_move_constructible<T>::value)
    {
        assert(m_Valid && "Don't access the Ok() value if it is an error, use IsOk() to check beforehand!");
        return std::move(m_Data);
    }

    inline const E& Err() const noexcept
    {
        assert(!m_Valid && "Don't access the Err() value if it is not an error, use IsErr() to check beforehand!");
        return m_Error;
    }

    inline E&& ErrTake() const noexcept(std::is_nothrow_move_constructible<E>::value)
    {
        assert(!m_Valid && "Don't access the Err() value if it is not an error, use IsErr() to check beforehand!");
        return std::move(m_Error);
    }

    explicit operator bool() const noexcept
    {
        return m_Valid;
    }

    inline bool IsOk() const noexcept
    {
        return m_Valid;
    }

    inline bool IsErr() const noexcept
    {
        return !m_Valid;
    }

    inline const T& Unwrap() const
    {
        if (m_Valid)
            return m_Data;
        throw m_Error;
    }

    inline T&& UnwrapTake() const
    {
        if (m_Valid)
            return std::move(m_Data);
        throw m_Error;
    }

    inline const T& UnwrapOr(const T& t) const noexcept
    {
        if (m_Valid)
            return m_Data;
        return t;
    }

    inline T&& UnwrapOrTake(T&& t) const noexcept(std::is_nothrow_move_constructible<T>::value)
    {
        if (m_Valid)
            return std::move(m_Data);
        return std::move(t);
    }

    template <typename U = T, typename std::enable_if<std::is_default_constructible<U>::value, int>::type = 0>
    inline T UnwrapOrDefault() const noexcept(std::is_nothrow_constructible<T>::value)
    {
        if (m_Valid)
            return m_Data;
        return T();
    }

    template <typename U = T, typename std::enable_if<std::is_default_constructible<U>::value, int>::type = 0>
    inline T&& UnwrapOrDefaultTake() const noexcept(std::is_nothrow_constructible<T>::value && std::is_nothrow_move_constructible<T>::value)
    {
        if (m_Valid)
            return std::move(m_Data);
        return T();
    }

    template <typename Func, typename... Args>
    inline T UnwrapOrElse(const Func& f, Args&&... args) const
    {
        if (m_Valid)
            return m_Data;
        return f(std::forward<Args>(args)...);
    }

    template <typename Func, typename... Args>
    inline T&& UnwrapOrElseTake(const Func& f, Args&&... args) const
    {
        if (m_Valid)
            return std::move(m_Data);
        return f(std::forward<Args>(args)...);
    }

    template <typename U = E, typename = ResultUtil::VoidT<decltype(std::declval<U>().What())>>
    inline const T& Expect(const char* msg) const
    {
        if (m_Valid)
            return m_Data;
        throw E(std::string(msg) + m_Error.what());
    }

    template <typename U = E, typename = ResultUtil::VoidT<decltype(std::declval<U>().What())>>
    inline T&& ExpectTake(const char* msg) const
    {
        if (m_Valid)
            return std::move(m_Data);
        throw E(std::string(msg) + m_Error.what());
    }
};


template <typename E>
class Result<void, E>
{
    static_assert(std::is_copy_assignable<E>::value, "Result<void>::Error has to be copy assignable");
    static_assert(std::is_move_assignable<E>::value, "Result<void>::Error type has to be move assignable");
    static_assert(std::is_copy_constructible<E>::value, "Result<void>::Error type has to be copiable");
    static_assert(std::is_move_constructible<E>::value, "Result<void>::Error type has to be movable");
private:
    mutable E m_Error;
    bool m_Valid;
public:
    inline Result(const E& e) : m_Error(e), m_Valid(false) {}
    inline Result() : m_Valid(true) {}

    Result(const Result& other) : m_Error(other.m_Error), m_Valid(other.m_Valid) {}
    Result(Result&& other) noexcept(std::is_nothrow_move_constructible<E>::value) : m_Valid(other.m_Valid), m_Error(std::move(other.m_Error)) {}

    Result& operator=(const Result& other)
    {
        if (this != &other)
        {
            m_Valid = other.m_Valid;
            m_Error = other.m_Error;
        }
        return *this;
    }

    Result& operator=(Result&& other) noexcept(std::is_nothrow_move_assignable<E>::value)
    {
        if (this != &other)
        {
            m_Valid = other.m_Valid;
            m_Error = std::move(other.m_Error);
        }
        return *this;
    }

    void operator=(const E& error)
    {
        m_Valid = false;
        m_Error = error;
    }

    inline const E& Err() const noexcept
    {
        return m_Error;
    }

    inline E&& ErrTake() const noexcept
    {
        assert(!m_Valid && "Don't access the Err() value if it is not an error, use IsErr() to check beforehand!");
        return std::move(m_Error);
    }

    explicit operator bool() const noexcept
    {
        return m_Valid;
    }

    inline bool IsOk() const noexcept
    {
        return m_Valid;
    }

    inline bool IsErr() const noexcept
    {
        return !m_Valid;
    }

    inline void Unwrap() const
    {
        if (!m_Valid)
            throw m_Error;
    }

    template <typename Func, typename... Args>
    inline void UnwrapOrElse(const Func& f, Args&&... args) const
    {
        if (!m_Valid)
            return f(std::forward<Args>(args)...);
    }

    template <typename U = E, typename = ResultUtil::VoidT<decltype(std::declval<U>().what())>>
    inline void Expect(const char* msg) const
    {
        if (!m_Valid)
            throw E(msg + m_Error.what());
    }
};

template <typename T, typename E = Err>
inline Result<T, E> Ok(T&& value)
{
    return Result<T, E>(std::forward<T>(value));
}

template <typename T, typename E = Err, typename... Args>
inline Result<T, E> Ok(Args&&... args)
{
    return Result<T, E>(T(std::forward<Args>(args)...));
}

template <typename E = Err>
inline Result<void, E> Ok()
{
    return Result<void, E>();
}
#endif // RESULT_H
