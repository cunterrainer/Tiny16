#ifndef SCREEN_HPP
#define SCREEN_HPP

#include <vector>
#include <cstdint>

#include "raylib.h"
#include "external/glad.h"

namespace UI
{
    class Screen
    {
    private:
        static constexpr int Width = 256;
        static constexpr int Height = 144;
    private:
        GLuint m_ImageTexture = 0;
        std::vector<std::uint8_t> m_Framebuffer;
    public:
        Screen() noexcept;
        ~Screen() noexcept;
        void Swap() const noexcept;

        constexpr GLuint GetTexture() const noexcept
        {
            return m_ImageTexture;
        }
    };
}

#endif // SCREEN_HPP