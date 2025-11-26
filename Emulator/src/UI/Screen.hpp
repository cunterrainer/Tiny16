#ifndef SCREEN_HPP
#define SCREEN_HPP

#include <cstdint>

#include "imgui.h"

#include "raylib.h"

namespace UI
{
    class Screen
    {
    public:
        static constexpr int Width = 256;
        static constexpr int Height = 144;
    private:
        Texture2D m_Texture;
    public:
        Screen(const std::uint8_t* const screenPixelPtr) noexcept;
        ~Screen() noexcept;
        void Swap(const std::uint8_t* const screenPixelPtr) const noexcept;

        constexpr ImU64 GetTexture() const noexcept
        {
            return m_Texture.id;
        }
    };
}

#endif // SCREEN_HPP