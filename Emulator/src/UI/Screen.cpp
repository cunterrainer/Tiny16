#include "UI/Screen.hpp"

namespace UI
{
    Screen::Screen(const std::uint8_t* const screenPixelPtr) noexcept
    {
        glGenTextures(1, &m_ImageTexture);
        glBindTexture(GL_TEXTURE_2D, m_ImageTexture);

        // Setup filtering parameters for display
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Setup filtering parameters for display
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_RED);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_RED);

        // Upload pixels into texture
        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, Width, Height, 0, GL_RED, GL_UNSIGNED_BYTE, screenPixelPtr);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    Screen::~Screen() noexcept
    {
        glBindTexture(GL_TEXTURE_2D, 0);
        if (m_ImageTexture != 0)
            glDeleteTextures(1, &m_ImageTexture);
    }

    void Screen::Swap(const std::uint8_t* const screenPixelPtr) const noexcept
    {
        glBindTexture(GL_TEXTURE_2D, m_ImageTexture);
        // Update GPU texture
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, Width, Height, GL_RED, GL_UNSIGNED_BYTE, screenPixelPtr);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}