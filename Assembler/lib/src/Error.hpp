#ifndef ERROR_HPP
#define ERROR_HPP

#include <utility>

#include "Utility/Result.hpp"

struct ASMError : Err
{
private:
    size_t m_LineNumber;
public:
    ASMError() {}
    template <typename... Args>
    explicit ASMError(size_t lineNumber, std::format_string<Args...> what, Args&&... args) : Err(what, std::forward<Args>(args)...), m_LineNumber(lineNumber) {}

    size_t LineNumber() const noexcept
    {
        return m_LineNumber;
    }
};

#endif // ERROR_HPP