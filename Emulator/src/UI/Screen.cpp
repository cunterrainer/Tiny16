#include "raylib.h"

#include "UI/Screen.hpp"

namespace UI
{
    Screen::Screen(const std::uint8_t* const screenPixelPtr) noexcept
    {
        // Construct a temporary Image wrapper around your raw pixel data.
        // We do this manually to avoid allocating new memory
        Image tempImage ;
        tempImage.data = (void*)screenPixelPtr; // Point to raw R8 bytes
        tempImage.width = Width;
        tempImage.height = Height;
        tempImage.mipmaps = 1;
        tempImage.format = PIXELFORMAT_UNCOMPRESSED_GRAYSCALE; // R8 Equivalent

        m_Texture = LoadTextureFromImage(tempImage);
        SetTextureFilter(m_Texture, TEXTURE_FILTER_BILINEAR);

        // NOTE: We do NOT call UnloadImage(tempImage) here because tempImage.data 
        // points to the emulator's memory, not memory allocated by Raylib.
    }

    Screen::~Screen() noexcept
    {
        UnloadTexture(m_Texture);
    }

    void Screen::Swap(const std::uint8_t* const screenPixelPtr) const noexcept
    {
        UpdateTexture(m_Texture, screenPixelPtr);
    }
}